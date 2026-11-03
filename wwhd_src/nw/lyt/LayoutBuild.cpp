// format facts: BFLYT block tags and binary field layouts; implementation reconstructed from WWHD.
/* Layout resource block construction, reconstructed from WWHD control flow and dispatch tables. */
#include "nw/lyt/LayoutBuild.h"
using namespace nw::lyt;
using nw::ut::HeapLink;
static void copyBuildFloat(be<f32>& destination,const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination)=static_cast<u32>(reinterpret_cast<const be<u32>&>(source));
}
/* 0287FF60 */
LayoutPartIndex* LayoutPartIndex::create(LayoutPartIndex* index,LayoutPane* parent,LayoutPartsResource* source,void* userData,const PaneSize* size) {
    gabi::AutoFrame frame{gabi::current_function()};
    if (!index) index=static_cast<LayoutPartIndex*>(sessionBuildAllocate(sizeof(LayoutPartIndex)));
    if (index) {
        u32 count=source->count;
        index->resource=reinterpret_cast<be<u8>*>(source);
        index->count=count;
        index->parent=parent;
        index->entries=source->entries;
        index->userData=userData;
        f32 parentWidth=parent->content()->width;
        f32 width=size->width;
        f32 horizontal=gabi::fdivs_ppc(parentWidth,width);
        f32 parentHeight=parent->content()->height;
        f32 sourceHeight=source->height;
        f32 height=size->height;
        f32 vertical=gabi::fdivs_ppc(parentHeight,height);
        f32 sourceWidth=source->width;
        index->scaleX=gabi::fmuls_ppc(horizontal,sourceWidth);
        index->scaleY=gabi::fmuls_ppc(vertical,sourceHeight);
    }
    return index;
}
/* 02880010 */
u32 Layout::buildFromResource(FontFile* file,ResourceProvider* provider,const LayoutBuildOptions* options,LayoutPartIndex* overrides) {
    gabi::AutoFrame frame{gabi::current_function()};
    mResources=provider;
    (void)layoutValidateResourceHeader(file,0x464C5954u,0x02020000u,1);
    gabi::FrameLocal<LayoutResourceSet> resourceSet(0x40);
    resourceSet->userData=nullptr;
    resourceSet->userData=this;
    resourceSet->fonts=nullptr;
    resourceSet->layout=nullptr;
    resourceSet->provider=nullptr;
    resourceSet->materials=nullptr;
    resourceSet->provider=provider;
    gabi::FrameLocal<LayoutBlockBuildContext> context(8);
    LayoutBuildPrefix* prefix=context->prefix();
    if (overrides) {
        copyBuildFloat(prefix->scaleY,overrides->scaleY);
        copyBuildFloat(prefix->scaleX,overrides->scaleX);
        context->overrideResources=reinterpret_cast<LayoutResourceSet*>(overrides->userData.get());
    } else {
        context->overrideResources=nullptr;
        prefix->scaleY=1.0f;
        prefix->scaleX=1.0f;
    }
    prefix->offsetY=0.0f;
    prefix->offsetX=0.0f;
    prefix->listener=options->listener.get();
    context->searchUserData=this;
    context->resources=resourceSet.get();
    context->textSearcher=options->textSearcher.get();
    context->currentParent=nullptr;
    context->flags=0;
    LayoutPane* lastPane=nullptr;
    bool groupsStarted=false;
    s32 groupDepth=0;
    s32 index=0;
    s32 count=static_cast<u16>(file->blockCount);
    FontBlock* block=layoutAtOffset<FontBlock>(file,file->headerSize);
    gabi::FrameLocal<gptr<HeapLink>> iterator(0x3C);
    gabi::FrameLocal<RelativeResourceTables> container(0x54);
    gabi::FrameLocal<LayoutPartIndex> partIndex(0x64);
    while (index<count) {
        FontBlock* activeBlock=block;
        u32 tag=activeBlock->tag;
        bool reloadCount=false;
        switch (tag) {
        case 0x6C797431: {
            auto* description=reinterpret_cast<LayoutDescriptionBlock*>(block);
            copyBuildFloat(mWidth,description->width);
            mResourceName=description->name;
            copyBuildFloat(mHeight,description->height);
            copyBuildFloat(prefix->offsetX,description->offsetX);
            copyBuildFloat(prefix->offsetY,description->offsetY);
            reloadCount=true;
            break;
        }
        case 0x636E7431: {
            LayoutBuildListener* listener=prefix->listener;
            if (listener) {
                container->attach(reinterpret_cast<RelativeTableHeader*>(block));
                listener=prefix->listener;
                gabi::call_ptr<void>(listener->methods->container,listener,container.get(),this);
                reloadCount=true;
            }
            break;
        }
        case 0x74786C31: resourceSet->layout=block; reloadCount=true; break;
        case 0x666E6C31: resourceSet->fonts=reinterpret_cast<ResourceNameTable*>(block); reloadCount=true; break;
        case 0x6D617431: resourceSet->materials=reinterpret_cast<ResourceNameTable*>(block); reloadCount=true; break;
        case 0x626E6431:
        case 0x70616E31:
        case 0x70696331:
        case 0x70727431:
        case 0x74787431:
        case 0x776E6431: {
            context->flagPair()->secondary=0;
            context->partFlags=nullptr;
            context->userDataBlock=nullptr;
            context->flags=0;
            FontBlock* source=block;
            void* alternate=nullptr;
            if (index+1<static_cast<s32>(static_cast<u16>(file->blockCount))) {
                FontBlock* next=layoutAtOffset<FontBlock>(block,activeBlock->size);
                if (next->tag==0x75736431u) {
                    activeBlock=next;
                    block=next;
                    ++index;
                    context->userDataBlock=next;
                }
            }
            LayoutPane* pane=nullptr;
            bool reuse=false;
            if (overrides) {
                if (mRoot) {
                    LayoutPartResource* entry=overrides->find(reinterpret_cast<PaneResource*>(source)->name);
                    if (entry) {
                        alternate=overrides->paneResource(entry);
                        context->flags=entry->primaryFlags;
                        context->flagPair()->secondary=entry->secondaryFlags;
                        context->partFlags=overrides->flags(entry);
                        void* userBlock=overrides->alternate(entry);
                        if (userBlock) context->userDataBlock=static_cast<FontBlock*>(userBlock);
                    }
                } else {
                    pane=overrides->parent;
                    reuse=pane!=nullptr;
                }
            }
            if (reuse) {
                mRoot=pane;
            } else {
                auto* dispatch=mMethods.get();
                pane=gabi::call_ptr<LayoutPane*>(dispatch->createPane,this,tag,source,alternate,context.get());
                if (pane && !mRoot) mRoot=pane;
            }
            if (pane) {
                LayoutPane* parent=context->currentParent;
                if (parent) parent->appendChild(pane);
                if (tag==0x70727431u) {
                    auto* selected=reinterpret_cast<LayoutPartsResource*>(alternate?alternate:source);
                    void* userData=alternate?static_cast<void*>(context->overrideResources.get()):static_cast<void*>(resourceSet.get());
                    LayoutPartIndex::create(partIndex.get(),pane,selected,userData,&reinterpret_cast<PaneResource*>(source)->paneSize);
                    auto* dispatch=mMethods.get();
                    auto* childName=selected->layoutName();
                    Layout* child=gabi::call_ptr<Layout*>(dispatch->createNested,this,childName,partIndex.get(),context.get());
                    if (child) {
                        *iterator=&mParts.root;
                        auto* part=static_cast<LayoutParts*>(pane);
                        part->nestedLayout=reinterpret_cast<hdLayout_c*>(child);
                        sessionBuildInsert(&mParts,iterator.get(),partLink(part));
                    }
                }
                lastPane=pane;
            }
            reloadCount=true;
            break;
        }
        case 0x70617331: context->currentParent=lastPane; reloadCount=true; break;
        case 0x70616531: {
            LayoutPane* parent=context->currentParent;
            lastPane=parent;
            context->currentParent=parent->content()->parent.get();
            reloadCount=true;
            break;
        }
        case 0x67727031:
            if (!groupsStarted) {
                groupsStarted=true;
                auto* list=static_cast<PaneGroupList*>(allocateLayoutStorage(12,4));
                if (list) {
                    list->count=0;
                    list->root.previous=&list->root;
                    list->root.next=&list->root;
                }
                mGroups=list;
                reloadCount=true;
            } else if (mGroups && groupDepth==1) {
                LayoutPane* pane=mRoot;
                auto* group=static_cast<NamedPaneGroup*>(allocateLayoutStorage(0x34,4));
                if (group) {
                    group=NamedPaneGroup::createFromResource(group,reinterpret_cast<PaneGroupResource*>(activeBlock),reinterpret_cast<hdLayout_lookup_c*>(pane));
                    if (group) mGroups->append(group);
                }
                reloadCount=true;
            }
            break;
        case 0x67727331: ++groupDepth; break;
        case 0x67726531: --groupDepth; break;
        }
        if (reloadCount) count=static_cast<u16>(file->blockCount);
        u32 size=activeBlock->size;
        ++index;
        block=layoutAtOffset<FontBlock>(block,size);
    }
    return 1;
}
/* 02880B4C */
Layout* Layout::createPart(const be<u8>* name,LayoutPartIndex* overrides,const LayoutBuildOptions* options) {
    gabi::AutoFrame frame{gabi::current_function()};
    void* resource=findLayoutResource(name);
    auto* memory=static_cast<Layout*>(allocateLayoutStorage(0x34,4));
    Layout* child=nullptr;
    if (memory) child=Layout::create(memory);
    // child->mMethods, read through the guest address: the original also dispatches when the allocation failed
    // (a null child), so the load must not let the compiler assume child is non-null.
    LayoutSessionMethods* dispatch=*layoutAtOffset<gptr<LayoutSessionMethods>>(child,offsetof(Layout,mMethods));
    ResourceProvider* provider=mResources;
    gabi::call_ptr<void>(dispatch->build,child,resource,provider,options,overrides);
    return child;
}
/* WWHD glue
ext 0273AD10 void* sessionBuildAllocate(u32)
ext 0286DA70 void sessionBuildInsert(nw::ut::HeapChildren*,gptr<nw::ut::HeapLink>*,void*)
ext 02873CE8 void nw::lyt::RelativeResourceTables::attach(nw::lyt::RelativeTableHeader*)
ext 02876524 void nw::lyt::LayoutPane::appendChild(nw::lyt::LayoutPane*)
ext 028745B0 nw::lyt::NamedPaneGroup* nw::lyt::NamedPaneGroup::createFromResource(nw::lyt::NamedPaneGroup*,nw::lyt::PaneGroupResource*,hdLayout_lookup_c*)
ext 028747F0 void nw::lyt::PaneGroupList::append(nw::lyt::NamedPaneGroup*)
*/
