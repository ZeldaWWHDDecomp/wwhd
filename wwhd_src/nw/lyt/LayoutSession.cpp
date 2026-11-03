/* Layout ownership, traversal and resource factories reconstructed from WWHD. */
#include "nw/lyt/LayoutSession.h"
#include "nw/lyt/LayoutResourceFormatting.h"
using namespace nw::lyt;
using nw::ut::HeapLink;
static u8 sLayoutMethods;
struct SessionPaneMethods {
    u8 prefix[0xC]; be<u32> destroy; u8 destroyGap[4]; be<u32> runtimeType;
    u8 searchGap[0x6C]; be<u32> update; u8 updateGap[4]; be<u32> traverse;
};
struct SessionAnimationMethods { u8 prefix[0x14]; be<u32> destroy; u8 gap[0x14]; be<u32> apply; };
struct SessionAnimation { HeapLink link; u8 prefix[0xC]; gptr<SessionAnimationMethods> methods; };
struct SessionPart { HeapLink link; gptr<Layout> session; };
struct SessionPartMethods { u8 prefix[0x4C]; be<u32> apply; };
/* 0287F418 */
Layout* Layout::create(Layout* layout) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!layout) layout=static_cast<Layout*>(sessionAllocate(sizeof(Layout)));
    if (layout) {
        layout->mMethods=reinterpret_cast<LayoutSessionMethods*>(&sLayoutMethods);
        layout->mAnimators.count=0;
        layout->mAnimators.root.previous=&layout->mAnimators.root;
        layout->mAnimators.root.next=&layout->mAnimators.root;
        layout->mResources=nullptr;
        layout->mGroups=nullptr;
        layout->mResourceName=nullptr;
        layout->mWidth=0.0f;
        layout->mRoot=nullptr;
        layout->mHeight=0.0f;
        layout->mParts.count=0;
        layout->mParts.root.previous=&layout->mParts.root;
        layout->mParts.root.next=&layout->mParts.root;
    }
    return layout;
}
/* 0287F4E0 */
void Layout::destroy(u32 flags) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!this) return;
    PaneGroupList* ownedGroups=mGroups;
    mMethods=reinterpret_cast<LayoutSessionMethods*>(&sLayoutMethods);
    if (ownedGroups) { ownedGroups->destroy(2); releaseLayoutStorage(ownedGroups); }
    LayoutPane* pane=mRoot;
    if (pane && !(static_cast<u32>(pane->content()->flags)&8)) {
        auto* dispatch=paneMethods<SessionPaneMethods>(pane);
        gabi::call_ptr<void>(dispatch->destroy,pane,2);
        releaseLayoutStorage(pane);
    }
    HeapLink* sentinel=&mAnimators.root;
    HeapLink* next=sentinel->next;
    gabi::FrameLocal<gptr<HeapLink>> iterator(8);
    while (next!=sentinel) {
        HeapLink* current=next;
        next=current->next;
        *iterator=current;
        sessionErase(&mAnimators,iterator.get());
        if (current) {
            auto* animation=reinterpret_cast<SessionAnimation*>(current);
            gabi::call_ptr<void>(animation->methods->destroy,animation,2);
            releaseLayoutStorage(animation);
        }
    }
    if (flags&1) sessionDelete(this);
}
/* 0287F5DC */
u32 Layout::build(void* resource,ResourceProvider* provider,LayoutBuildListener* listener,TextSearcher* textSearcher) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<LayoutBuildOptions> options(8);
    options->reserved=0;
    options->offsetX=0.0f;
    options->scaleY=1.0f;
    options->offsetY=0.0f;
    options->scaleX=1.0f;
    auto* dispatch=mMethods.get();
    options->textSearcher=textSearcher;
    options->listener=listener;
    return gabi::call_ptr<u32>(dispatch->build,this,resource,provider,options.get(),0);
}
/* 0287F640 */
u32 Layout::buildByName(const be<u8>* name,ResourceProvider* provider,LayoutBuildListener* listener,TextSearcher* textSearcher) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* resource=gabi::call_ptr<void*>(provider->methods->findResource,provider,0x626C7974u,name,0);
    if (!resource) return 0;
    return build(resource,provider,listener,textSearcher);
}
/* 0287F6D0 */
void* Layout::findAnimationResource(const be<u8>* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u8>[0x88]> path(8);
    const be<u8>* base=mResourceName;
    gabi::cpu->cr[6]=0;
    formatLayoutName(&(*path)[0],0x88,0x87,"%s_%s.bflan",gabi::ea(base),gabi::ea(name),gabi::cpu->r[9],gabi::cpu->r[10]);
    ResourceProvider* provider=mResources;
    return gabi::call_ptr<void*>(provider->methods->findResource,provider,0x616E696Du,path.get(),0);
}
/* 02880AE0 */
void* Layout::findLayoutResource(const be<u8>* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    gabi::FrameLocal<be<u8>[0x48]> path(8);
    gabi::cpu->cr[6]=0;
    formatLayoutName(&(*path)[0],0x48,0x47,"%s.bflyt",gabi::ea(name),gabi::cpu->r[8],gabi::cpu->r[9],gabi::cpu->r[10]);
    ResourceProvider* provider=mResources;
    return gabi::call_ptr<void*>(provider->methods->findResource,provider,0x626C7974u,path.get(),0);
}
/* 0287FAD4 */
void Layout::applyAnimations() {
    gabi::AutoFrame frame{gabi::current_function()};
    HeapLink* sentinel=&mAnimators.root;
    HeapLink* current=sentinel->next;
    while (current!=sentinel) {
        auto* animation=reinterpret_cast<SessionAnimation*>(current);
        gabi::call_ptr<void>(animation->methods->apply,animation);
        current=current->next;
    }
    sentinel=&mParts.root;
    current=sentinel->next;
    while (current!=sentinel) {
        auto* part=reinterpret_cast<SessionPart*>(current);
        Layout* child=part->session;
        auto* dispatch=reinterpret_cast<SessionPartMethods*>(child->mMethods.get());
        gabi::call_ptr<void>(dispatch->apply,child);
        current=current->next;
    }
}
/* 0287FBFC */
void Layout::updateMatrices(LayoutDrawContext* context,u32 force) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mRoot) {
        gabi::FrameLocal<PaneUpdateContext> state(8);
        PaneUpdateContext::create(state.get(),context);
        LayoutPane* pane=mRoot;
        auto* dispatch=paneMethods<SessionPaneMethods>(pane);
        gabi::call_ptr<void>(dispatch->update,pane,state.get(),force);
    }
}
/* 0287FC60 */
void Layout::traverse(LayoutDrawContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (mRoot) {
        // The traversal stores a temporary layout owner at the word immediately after the renderer pointer.
        context->stateWord=gabi::ea(this);
        context->flagB3=6;
        context->vertexBindingPending=1;
        context->flagB5=0;
        context->flagB4=0;
        LayoutPane* pane=mRoot;
        auto* dispatch=paneMethods<SessionPaneMethods>(pane);
        gabi::call_ptr<void>(dispatch->traverse,pane,context);
        context->stateWord=0;
    }
}
/* 0287FCD8 */
void nw::lyt::setPaneTagProcessor(LayoutPane* pane,WideTagProcessor* replacement) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!static_cast<u32>(sTextPaneTypeGuard.ready)) {
        u32 baseReady=sPaneTypeGuard.ready;
        sTextPaneTypeGuard.ready=1;
        if (!baseReady) {
            sPaneTypeGuard.ready=1;
            sPaneType.parent=nullptr;
        }
        sTextPaneType.parent=&sPaneType;
    }
    if (pane) {
        auto* dispatch=paneMethods<SessionPaneMethods>(pane);
        RuntimeTypeNode* type=gabi::call_ptr<RuntimeTypeNode*>(dispatch->runtimeType,pane);
        while (type && type!=&sTextPaneType) type=type->parent;
        if (type) {
            TextPane* text=static_cast<TextPane*>(pane);
            bool changed=text->tagProcessor.get()!=replacement;
            u32 flags=text->textFlags;
            text->textFlags=flags|(changed?TextPane_Dirty:0);
            if (changed) text->tagProcessor=replacement;
        }
    }
    // The children's sentinel (content()->children.root), addressed from the guest pointer: the original walks the
    // list even for a null pane.
    HeapLink* sentinel=layoutAtOffset<HeapLink>(pane,offsetof(LayoutPane,_0C)+offsetof(PaneContent,children.root));
    HeapLink* child=sentinel->next;
    while (child!=sentinel) {
        setPaneTagProcessor(reinterpret_cast<LayoutPane*>(child),replacement);
        child=child->next;
    }
}
/* 0287FE08 */
void Layout::setTagProcessor(WideTagProcessor* processor) {
    gabi::tail_ptr<void>(gabi::guest_fn((const void*)setPaneTagProcessor),mRoot.get(),processor);
}
/* 0287FE10 */
LayoutParts* Layout::findParts(const be<u8>* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    HeapLink* sentinel=&mParts.root;
    HeapLink* current=sentinel->next;
    while (current!=sentinel) {
        LayoutParts* part=partFromLink(current);
        if (sessionShortNameEqual(name,part->content()->name)) return part;
        current=current->next;
    }
    return nullptr;
}
/* 0287FE9C */
LayoutPartResource* LayoutPartIndex::find(const be<u8>* name) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 remaining=count;
    if (!remaining) return nullptr;
    LayoutPartResource* entry=entries;
    for (u32 i=0;i<remaining;++i) {
        if (sessionShortNameEqual(name,entry->name)) return entry;
        ++entry;
    }
    return nullptr;
}
/* 0287FF0C */
void* LayoutPartIndex::alternate(LayoutPartResource* entry) {
    u32 offset=entry->alternateOffset;
    return offset?layoutAtOffset<void>(resource,offset):nullptr;
}
/* 0287FF28 */
void* LayoutPartIndex::paneResource(LayoutPartResource* entry) {
    u32 offset=entry->resourceOffset;
    return offset?layoutAtOffset<void>(resource,offset):nullptr;
}
/* 0287FF44 */
void* LayoutPartIndex::flags(LayoutPartResource* entry) {
    u32 offset=entry->flagsOffset;
    return offset?layoutAtOffset<void>(resource,offset):nullptr;
}
/* 028807D0 */
LayoutPane* Layout::createPane(u32 tag,void* resource,void* alternate,PaneBuildContext* context) {
    gabi::AutoFrame frame{gabi::current_function()};
    u32 size;
    switch (tag) {
    case 0x70616E31: size=0xA4; break;
    case 0x70696331: size=0xC0; break;
    case 0x74787431: size=0x104; break;
    case 0x776E6431: size=0xD8; break;
    case 0x626E6431: size=0xA4; break;
    case 0x70727431: size=0xB0; break;
    default: return nullptr;
    }
    void* memory=allocateLayoutStorage(size,4);
    if (!memory) return nullptr;
    switch (tag) {
    case 0x70616E31: return LayoutPane::createFromResource(static_cast<LayoutPane*>(memory),resource,context);
    case 0x70696331: return reinterpret_cast<LayoutPane*>(LayoutPicture::create(static_cast<LayoutPicture*>(memory),resource,alternate,context));
    case 0x74787431: return TextPane::createFromResource(static_cast<TextPane*>(memory),static_cast<TextBoxResource*>(resource),static_cast<TextBoxResource*>(alternate),context);
    case 0x776E6431: return WindowPane::createFromResource(static_cast<WindowPane*>(memory),static_cast<WindowResource*>(resource),static_cast<WindowResource*>(alternate),context);
    case 0x626E6431: return LayoutBounding::create(static_cast<LayoutBounding*>(memory),resource,alternate,context);
    default: return LayoutParts::create(static_cast<LayoutParts*>(memory),resource,alternate,context);
    }
}
/* WWHD glue
str 10181954 "%s_%s.bflan"
str 10181960 "%s.bflyt"
data sLayoutMethods 1018196C
ext 0273AD10 void* sessionAllocate(u32)
ext 0273AF40 void sessionDelete(void*)
ext 0286DB18 void sessionErase(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*)
ext 0286DA70 void sessionInsert(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,void*)
ext 02872920 u32 sessionShortNameEqual(const be<u8>*,const be<u8>*)
ext 0287474C void nw::lyt::PaneGroupList::destroy(u32)
ext 02877358 nw::lyt::PaneUpdateContext* nw::lyt::PaneUpdateContext::create(nw::lyt::PaneUpdateContext*,nw::lyt::LayoutDrawContext*)
ext 02875C80 nw::lyt::LayoutPane* nw::lyt::LayoutPane::createFromResource(nw::lyt::LayoutPane*,void*,void*)
ext 02874AAC nw::lyt::LayoutPicture* nw::lyt::LayoutPicture::create(nw::lyt::LayoutPicture*,void*,void*,void*)
ext 0287795C nw::lyt::TextPane* nw::lyt::TextPane::createFromResource(nw::lyt::TextPane*,nw::lyt::TextBoxResource*,nw::lyt::TextBoxResource*,nw::lyt::PaneBuildContext*)
ext 02878F9C nw::lyt::WindowPane* nw::lyt::WindowPane::createFromResource(nw::lyt::WindowPane*,nw::lyt::WindowResource*,nw::lyt::WindowResource*,nw::lyt::PaneBuildContext*)
ext 028757F0 nw::lyt::LayoutBounding* nw::lyt::LayoutBounding::create(nw::lyt::LayoutBounding*,void*,void*,void*)
ext 02874894 nw::lyt::LayoutParts* nw::lyt::LayoutParts::create(nw::lyt::LayoutParts*,void*,void*,void*)
*/
