/* Pane defaults, names and user data reconstructed from WWHD instructions.
 * The full allocation and member offsets are constructor-proven; merged HD storage stays unchanged. */
#include "nw/lyt/PaneState.h"
using namespace nw::lyt;
static u8 sPaneLinkMethods;
static u8 sPaneMethods;
struct PaneIdentityGuard { be<u32> ready; };
static PaneIdentityGuard sPaneIdentityGuard;
static Mtx34 sPaneIdentity;
struct PaneColorMethods { u8 prefix[0x44]; be<u32> setVertexColor; };

/* A float the original loads and stores again with no other memory access in between keeps its
 * exact bits in the recompiled reference (a signalling NaN is not quieted); a float that waits in
 * an FPR across another load or store is quieted like any f32 value. */
static inline void copyFloatBits(be<f32>& destination, const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination) = (u32) reinterpret_cast<const be<u32>&>(source);
}

/* 0287590C */
PaneLinkPrefix* PaneLinkPrefix::create(PaneLinkPrefix* pane) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<PaneLinkPrefix*>(paneStateAllocate(sizeof(PaneLinkPrefix)));
    if (pane) {
        pane->link.previous = nullptr;
        pane->methods = gabi::ea(&sPaneLinkMethods);
        pane->link.next = nullptr;
    }
    return pane;
}

/* 02875958 */
void LayoutPane::initializeState() {
    auto* state = content();
    state->parent = nullptr;
    state->flags = 0;
    state->userData = nullptr;
    if (u32(sPaneIdentityGuard.ready) == 0) {
        sPaneIdentityGuard.ready = 1;
        sPaneIdentity.m[0][2] = 0.0f;
        sPaneIdentity.m[1][2] = 0.0f;
        sPaneIdentity.m[1][0] = 0.0f;
        sPaneIdentity.m[0][1] = 0.0f;
        sPaneIdentity.m[1][3] = 0.0f;
        sPaneIdentity.m[0][0] = 1.0f;
        sPaneIdentity.m[2][0] = 0.0f;
        sPaneIdentity.m[1][1] = 1.0f;
        sPaneIdentity.m[2][1] = 0.0f;
        sPaneIdentity.m[2][2] = 1.0f;
        sPaneIdentity.m[2][3] = 0.0f;
        sPaneIdentity.m[0][3] = 0.0f;
    }
    if (gabi::ea(&state->matrix) != gabi::ea(&sPaneIdentity)) {
        auto* source = reinterpret_cast<const be<u32>*>(&sPaneIdentity);
        auto* destination = reinterpret_cast<be<u32>*>(&state->matrix);
        for (u32 index = 0; index < 12; ++index) {
            u32 word = source[index];
            destination[index] = word;
        }
    }
    state->userPointer = nullptr;
}

static void initializePaneChildren(nw::ut::HeapChildren* children) {
    if (gabi::ea(children) == 0) children = static_cast<nw::ut::HeapChildren*>(paneStateAllocate(sizeof(nw::ut::HeapChildren)));
    if (children) {
        children->count = 0;
        children->root.previous = &children->root;
        children->root.next = &children->root;
    }
}

/* 02875A10 */
LayoutPane* LayoutPane::createDefault(LayoutPane* pane) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<LayoutPane*>(paneStateAllocate(sizeof(LayoutPane)));
    if (pane) {
        PaneLinkPrefix::create(reinterpret_cast<PaneLinkPrefix*>(pane));
        pane->mVtable = gabi::ea(&sPaneMethods);
        auto* state = pane->content();
        initializePaneChildren(&state->children);
        state->height = 0.0f;
        state->width = 0.0f;
        pane->initializeState();
        state->matrixFlag = 0;
        paneStateClearMemory(state->name, 0, 25);
        paneStateClearMemory(state->userInfo, 0, 9);
        u8 flags = state->flags;
        state->translation[2] = 0.0f;
        state->translation[1] = 0.0f;
        state->width = 0.0f;
        state->scale[0] = 1.0f;
        state->translation[0] = 0.0f;
        state->scale[1] = 1.0f;
        state->height = 0.0f;
        state->inheritedAlpha = 0xFF;
        state->rotation[2] = 0.0f;
        state->flags = ((flags & 0xFE) + 1) | 0x10;
        state->rotation[0] = 0.0f;
        state->rotation[1] = 0.0f;
        state->alpha = 0xFF;
    }
    return pane;
}

/* 02875B3C */
void LayoutPane::setName(const be<u8>* text) {
    auto* destination = content()->name;
    for (u32 index = 0; index < 24; ++index) {
        u8 character = *text++;
        if (character == 0) break;
        *destination++ = character;
    }
    *destination = 0;
}

/* 02875B70 */
void LayoutPane::setUserInfo(const be<u8>* text) {
    auto* destination = content()->userInfo;
    for (u32 index = 0; index < 8; ++index) {
        u8 character = *text++;
        if (character == 0) break;
        *destination++ = character;
    }
    *destination = 0;
}

/* 02876404 */
void LayoutPane::setColorElement(u32 index, u32 value) {
    if (index == 16) { content()->alpha = value; return; }
    auto* dispatch = paneMethods<PaneColorMethods>(this);
    gabi::tail_ptr<void>(dispatch->setVertexColor, this, index, value);
}

/* 028764A0 */
u32 LayoutPane::userDataCount() const {
    PaneUserData* data = content()->userData;
    return data ? u32(data->count) : 0;
}

/* 028764B8 */
void* LayoutPane::userDataEntries() const {
    PaneUserData* data = content()->userData;
    return data ? data->entries : nullptr;
}

/* 02875FC4 */
LayoutPane* LayoutPane::copyFrom(LayoutPane* pane, LayoutPane* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<LayoutPane*>(paneStateAllocate(sizeof(LayoutPane)));
    if (pane) {
        PaneLinkPrefix::create(reinterpret_cast<PaneLinkPrefix*>(pane));
        auto* state = pane->content();
        state->parent = nullptr;
        pane->mVtable = gabi::ea(&sPaneMethods);
        initializePaneChildren(&state->children);
        auto* original = source->content();
        auto* input = reinterpret_cast<const be<u32>*>(original->translation);
        auto* output = reinterpret_cast<be<u32>*>(state->translation);
        for (u32 index = 0; index < 8; ++index) {
            u32 word = input[index];
            output[index] = word;
        }
        copyFloatBits(state->width, original->width);
        copyFloatBits(state->height, original->height);
        state->flags = original->flags;
        state->alpha = original->alpha;
        state->inheritedAlpha = original->inheritedAlpha;
        state->matrixFlag = original->matrixFlag;
        if (gabi::ea(&state->matrix) != gabi::ea(&original->matrix)) {
            input = reinterpret_cast<const be<u32>*>(&original->matrix);
            output = reinterpret_cast<be<u32>*>(&state->matrix);
            for (u32 index = 0; index < 12; ++index) {
                u32 word = input[index];
                output[index] = word;
            }
        }
        state->userPointer = nullptr;
        state->userData = original->userData;
        pane->setName(original->name);
        pane->setUserInfo(original->userInfo);
        u8 flags = state->flags;
        state->flags = flags | 0x10;
    }
    return pane;
}

struct PaneOrigin { be<f32> x, y; };
struct PaneOriginWords { be<u32> x, y; };
struct PaneMaterialMethods {
    u8 prefix[0x4C];
    be<u32> material;
    u8 padding[4];
    be<u32> count;
};

/* 028762C8 */
Pair32 LayoutPane::originOffset() const {
    auto frame = gabi::AutoFrame(gabi::current_function());
    const auto* state = content();
    u8 modes = state->matrixFlag;
    u32 horizontalMode = modes & 3;
    f32 horizontal = 0.0f;
    if (horizontalMode == 0) horizontal = -gabi::fmuls_ppc(state->width, 0.5f);
    else if (horizontalMode == 2) horizontal = -f32(state->width);
    u32 verticalMode = (modes >> 2) & 3;
    f32 vertical = 0.0f;
    if (verticalMode == 0) vertical = gabi::fmuls_ppc(state->height, 0.5f);
    else if (verticalMode == 2) vertical = state->height;
    gabi::FrameLocal<PaneOrigin> origin(8);
    origin->x = horizontal;
    origin->y = vertical;
    auto* words = reinterpret_cast<PaneOriginWords*>(origin.get());
    return Pair32{words->x, words->y};
}

/* 02876368 */
void LayoutPane::bounds(TextBounds* output) const {
    auto frame = gabi::AutoFrame(gabi::current_function());
    TextBounds* storage = output;
    if (!storage) storage = static_cast<TextBounds*>(paneStateAllocate(sizeof(TextBounds)));
    if (storage) {
        storage->bottom = 0.0f;
        storage->left = 0.0f;
        storage->right = 0.0f;
        storage->top = 0.0f;
    }
    auto offset = originOffset();
    gabi::FrameLocal<PaneOriginWords> words(8);
    words->x = offset.r3;
    f32 left = gabi::f32_from_bits(words->x);
    words->y = offset.r4;
    f32 top = gabi::f32_from_bits(words->y);
    output->left = left;
    output->top = top;
    f32 width = content()->width;
    output->right = gabi::fadds_ppc(left, width);
    f32 height = content()->height;
    output->bottom = gabi::fsubs_ppc(top, height);
}

/* 02876424 */
void LayoutPane::setVertexColorElement(u32 index, u32 value) {}

/* 02876428 */
void* LayoutPane::firstMaterial() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* dispatch = paneMethods<PaneMaterialMethods>(this);
    u32 count = gabi::call_ptr<u32>(dispatch->count, this);
    if (count == 0) return nullptr;
    dispatch = paneMethods<PaneMaterialMethods>(this);
    return gabi::call_ptr<void*>(dispatch->material, this, 0u);
}

/* 02876498 */
void* LayoutPane::materialAt(u32 index) const { return nullptr; }

/* 028764CC */
void LayoutPane::insertChild(gptr<nw::ut::HeapLink>* position, LayoutPane* child) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> iterator(8);
    *iterator = *position;
    paneChildrenInsert(&content()->children, iterator.get(), reinterpret_cast<nw::ut::HeapLink*>(child));
    auto* state = child->content();
    u8 flags = state->flags;
    state->parent = this;
    state->flags = flags | 0x10;
}

/* 02876524 */
void LayoutPane::appendChild(LayoutPane* child) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<nw::ut::HeapLink>> position(8);
    *position = &content()->children.root;
    insertChild(position.get(), child);
}


struct PaneResourceState {
    u8 prefix[8]; be<u8> flags, matrixFlag, alpha, resizeFlags;
    be<u8> name[24], userInfo[8];
    be<f32> translation[3], rotation[3], scale[2], size[2];
};
struct PaneResourceOverride {
    be<u8> userInfo[8]; be<f32> translation[3], rotation[3], scale[2], size[2]; be<u8> alpha;
};
struct PaneBuildContext {
    be<f32> scale[2], size[2]; u8 prefix10[0x10];
    be<u32> enabled; u8 prefix24[2]; be<u16> overrideFlags;
    gptr<PaneResourceOverride> overrideState;
    gptr<LayoutPane> parent; gptr<PaneUserData> userData;
};
struct PaneAncestorScale { be<f32> values[2]; };

/* 02875C80 */
LayoutPane* LayoutPane::createFromResource(LayoutPane* pane, void* input, void* buildContext) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* resource = static_cast<PaneResourceState*>(input);
    auto* context = static_cast<PaneBuildContext*>(buildContext);
    if (!pane) pane = static_cast<LayoutPane*>(paneStateAllocate(sizeof(LayoutPane)));
    if (!pane) return pane;
    PaneLinkPrefix::create(reinterpret_cast<PaneLinkPrefix*>(pane));
    pane->mVtable = gabi::ea(&sPaneMethods);
    auto* state = pane->content();
    initializePaneChildren(&state->children);
    state->height = 0.0f;
    state->width = 0.0f;
    pane->initializeState();
    const be<f32>* translation = resource->translation;
    const be<f32>* rotation = resource->rotation;
    const be<f32>* scale = resource->scale;
    const be<f32>* size = resource->size;
    const be<u8>* alpha = &resource->alpha;
    const be<u8>* info = resource->userInfo;
    PaneResourceOverride* alternate = context->overrideState;
    if (alternate) {
        u32 flags = context->overrideFlags;
        if (flags & 8) translation = alternate->translation;
        if (flags & 0x40) rotation = alternate->rotation;
        if (flags & 0x20) scale = alternate->scale;
        if (flags & 0x10) size = alternate->size;
        if (flags & 0x80) alpha = &alternate->alpha;
        if (flags & 4) info = alternate->userInfo;
    }
    state->matrixFlag = resource->matrixFlag;
    pane->setName(resource->name);
    pane->setUserInfo(info);
    state->userData = context->userData;
    f32 tx = translation[0];
    f32 ty = translation[1];
    f32 tz = translation[2];
    state->translation[1] = ty;
    state->translation[0] = tx;
    state->translation[2] = tz;
    f32 rz = rotation[2];
    f32 rx = rotation[0];
    f32 ry = rotation[1];
    state->rotation[0] = rx;
    state->rotation[2] = rz;
    state->rotation[1] = ry;
    f32 sx = scale[0];
    f32 sy = scale[1];
    state->scale[0] = sx;
    state->scale[1] = sy;
    bool resize = false;
    if (u32(context->enabled) != 0) {
        f32 x = context->scale[0];
        if (x != 1.0f || f32(context->scale[1]) != 1.0f) {
            u32 flags = resource->resizeFlags;
            resize = !(flags & 1);
            if (resize && (flags & 2)) {
                u32 origin = state->matrixFlag;
                f32 factorX = (origin & 3) == 0 ? 1.0f : 0.5f;
                f32 factorY = ((origin >> 2) & 3) == 0 ? 1.0f : 0.5f;
                LayoutPane* ancestor = context->parent;
                gabi::FrameLocal<PaneAncestorScale> inherited(8);
                pane->ancestorScale(inherited->values, ancestor);
                f32 contextScaleX = context->scale[0];
                f32 inheritedX = inherited->values[0];
                f32 contextSizeX = context->size[0];
                f32 selectedScaleX = scale[0];
                f32 deltaX = gabi::fmsubs(contextSizeX, contextScaleX, contextSizeX);
                f32 contextSizeY = context->size[1];
                f32 denominatorX = gabi::fmuls_ppc(inheritedX, selectedScaleX);
                f32 contextScaleY = context->scale[1];
                f32 numeratorX = gabi::fmuls_ppc(deltaX, factorX);
                f32 deltaY = gabi::fmsubs(contextSizeY, contextScaleY, contextSizeY);
                f32 adjustmentX = gabi::fdivs_ppc(numeratorX, denominatorX);
                f32 selectedWidth = size[0];
                state->width = gabi::fadds_ppc(selectedWidth, adjustmentX);
                f32 selectedScaleY = scale[1];
                f32 numeratorY = gabi::fmuls_ppc(deltaY, factorY);
                f32 inheritedY = inherited->values[1];
                f32 denominatorY = gabi::fmuls_ppc(inheritedY, selectedScaleY);
                f32 adjustmentY = gabi::fdivs_ppc(numeratorY, denominatorY);
                f32 selectedHeight = size[1];
                state->height = gabi::fadds_ppc(selectedHeight, adjustmentY);
            } else if (resize) {
                f32 width = size[0];
                state->width = gabi::fmuls_ppc(width, x);
                f32 height = size[1];
                f32 y = context->scale[1];
                state->height = gabi::fmuls_ppc(height, y);
            }
        }
    }
    if (!resize) {
        copyFloatBits(state->width, size[0]);
        copyFloatBits(state->height, size[1]);
    }
    u32 opacity = *alpha;
    state->alpha = opacity;
    state->inheritedAlpha = opacity;
    u32 flags = resource->flags;
    state->flags = flags;
    if (u32(context->enabled) != 0 && (u32(context->overrideFlags) & 1)) {
        flags &= 0xFE;
        state->flags = flags;
        u32 overrides = context->overrideFlags;
        flags |= u32((overrides & 2) != 0);
    }
    state->flags = flags | 0x10;
    return pane;
}

/* WWHD glue
ext 0286DA70 void paneChildrenInsert(nw::ut::HeapChildren*, gptr<nw::ut::HeapLink>*, nw::ut::HeapLink*)
ext 0273AD10 void* paneStateAllocate(u32)
ext C000A858 void paneStateClearMemory(void*, u32, u32)
data sPaneLinkMethods 101805C4 nocheck
data sPaneMethods 101805D4 nocheck
data sPaneIdentityGuard 101FD304 nocheck
data sPaneIdentity 101FE0C4 nocheck
*/
