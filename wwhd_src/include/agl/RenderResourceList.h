// Render method resource list: a sead pointer buffer over its own 256-entry inline storage, plus a revision word.
// Layout from the constructor 027D56E4 (0x414 bytes, vtable 1016C144); RenderQueue's RenderMethodResource_l is a view of it.
#pragma once
#include "wwhd.h"
#include "sead/FileDevice.h"
#include "sead/Foundation.h"
namespace agl { namespace render {
struct RenderResourceList {
    gptr<sead::DisposerVtable> mVtable;
    sead::PointerBuffer mEntries;
    gptr<void> mInlineStorage[256];
    be<u32> mRevision;
    void destroy(u32 flags);
    void sortEntries();
    u32 addUnique(void* entry);
    u32 removeAll(void* entry);
};
WWHD_SIZE(RenderResourceList,0x414);
WWHD_OFFSET(RenderResourceList,mEntries,4);
WWHD_OFFSET(RenderResourceList,mInlineStorage,0x10);
WWHD_OFFSET(RenderResourceList,mRevision,0x410);
RenderResourceList* createRenderResourceList(RenderResourceList* list);
} }
void attachRenderResourceName(agl::render::RenderResourceList*,const sead::SafeText*);
