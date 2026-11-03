/* Resource lookup interface and per-picture inline views, reconstructed from WWHD. */
#pragma once
#include "nw/lyt/GlyphWriter.h"
#include "nw/lyt/TextureBinding.h"
namespace nw { namespace lyt {
struct ResourceProviderMethods { u8 prefix[0x1C]; be<u32> findResource; };
struct ResourceProvider {
    gptr<ResourceProviderMethods> methods;
    static ResourceProvider* create(ResourceProvider* provider);
    void destroy(u32 flags);
    bool loadTexture(TextureResourceView* texture, const be<u8>* name);
    FontInterface* loadFont(const be<u8>* name);
};
struct VertexColor : ColorWord { static VertexColor* create(VertexColor* color); };
struct RuntimeTypeNode { gptr<RuntimeTypeNode> parent; };
struct RuntimeTypeGuard { be<u32> ready; };   // the guard of a lazily initialised runtime-type node
RuntimeTypeNode* pictureRuntimeType();
WWHD_SIZE(ResourceProvider, 4);
WWHD_SIZE(VertexColor, sizeof(ColorWord));
WWHD_SIZE(RuntimeTypeNode, 4);
WWHD_SIZE(RuntimeTypeGuard, 4);
WWHD_OFFSET(ResourceProviderMethods, findResource, 0x1C);
} }
void* providerAllocate(u32 size);
void providerDelete(void* object);
void* providerAlignedAllocate(u32 size, s32 alignment);
void providerRelease(void* storage);
u32 providerDecodeTexture(nw::lyt::TextureResourceView*, const void*, u32);
nw::lyt::ResourceFont* providerCreateResourceFont(nw::lyt::ResourceFont*);
u32 providerAttachResourceFont(nw::lyt::ResourceFont*, nw::lyt::FontFile*);

// The pane base class's runtime-type node (101FD8FC) and its guard (101FD888): one guarded static that every
// pane kind's constructor initialises inline. Owned by nw/lyt/PaneRuntimeType.cpp.
extern nw::lyt::RuntimeTypeGuard& sPaneTypeGuard;
extern nw::lyt::RuntimeTypeNode& sPaneType;
// The text box pane's runtime-type node (101FD8F8) and its guard (101FD89C): TextPane::runtimeType initialises it,
// and translation units that inline that accessor share it. Owned by nw/lyt/PaneRuntimeType.cpp.
extern nw::lyt::RuntimeTypeGuard& sTextPaneTypeGuard;
extern nw::lyt::RuntimeTypeNode& sTextPaneType;
