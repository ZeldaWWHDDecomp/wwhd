/* nw::lyt window pane: construction from a wnd1 block, frame and material ownership, material lookup,
 * and drawing of the content quad and the frames for the three window kinds (around, horizontal,
 * horizontal without content) with one, two, four or eight frame materials.
 * Written from WWHD's disassembly; layouts from its constructor and method table.
 * format facts: BFLYT wnd1 block (public format documentation; facts only, no text or code copied) */
#include "nw/lyt/WindowPane.h"
using namespace nw::lyt;

static constexpr u32 kMaterialSize = 0x3C;   // sizeof(LayoutMaterial)
static constexpr u32 kFrameMaterialsVisible = 0xC0000000;   // featureFlags: the material draws at all
static u8 sWindowPaneMethods;

enum WindowUniform : u32 {
    Uniform_FrameMode = 10, Uniform_FrameInsets = 11, Uniform_WindowSize = 12, Uniform_VertexColors = 13,
};
enum FrameModeBits : u32 { FrameMode_NoVertexColor = 0x40000000, FrameMode_VertexColorContent = 0xD00 };
struct UniformVector { be<f32> x, y, z, w; };
struct UniformWords { be<u32> x, y, z, w; };
struct UniformSize { be<f32> width, height; be<u32> z, w; };
struct UniformColors { be<f32> values[16]; };
struct FrameModeTable { be<u32> values[8]; };
struct TableGuard { be<u32> ready; };
struct WindowCorners { PaneSize values[4]; };
struct WindowRectangle { PaneSize position, size; };
struct WindowRectangles { WindowRectangle values[8]; };
WWHD_SIZE(UniformVector, 0x10);
WWHD_SIZE(WindowRectangles, 0x80);

static inline void storeWords(PaneSize* point, Pair32 words) {
    reinterpret_cast<be<u32>&>(point->width) = words.r3;
    reinterpret_cast<be<u32>&>(point->height) = words.r4;
}
static inline void copyFloatBits(be<f32>& destination, const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination) = (u32) reinterpret_cast<const be<u32>&>(source);
}
static inline void destroyOwnedMaterial(LayoutMaterial* material) {
    gabi::call_ptr<void>(materialMethods(material)->destroy, material, u32(2));
    textPaneRelease(material);
}

/* 02878ED0 */
void WindowPane::reserveCoordinates(u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (count) windowReserveCoordinates(&coordinates, count);
}

/* 02878EE0 */
void WindowPane::reserveFrames(u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    frameCount = 0;
    auto* entries = static_cast<WindowFrame*>(textPaneAlignedAllocate(count * sizeof(WindowFrame), 4));
    if (!entries) {
        frames = nullptr;
        return;
    }
    for (u32 index = 0; index < count; ++index) {
        WindowFrame* entry = entries + index;
        if (entry) {
            entry->material = nullptr;
            entry->textureFlip = 0;
        }
    }
    frames = entries;
    if (entries) frameCount = count;
}

/* 02878F9C */
WindowPane* WindowPane::createFromResource(WindowPane* pane, WindowResource* resource, WindowResource* overrideResource, PaneBuildContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<WindowPane*>(textPaneAllocate(sizeof(WindowPane)));
    if (!pane) return pane;
    LayoutPane::createFromResource(pane, resource, context);
    pane->mVtable = gabi::ea(&sWindowPaneMethods);
    textPaneConstructArray(pane->colors, 4, sizeof(ColorWord), gabi::guest_fn((const void*)windowCreateColor));
    windowCreateCoordinates(&pane->coordinates);
    pane->kind = Window_Around;
    pane->windowFlags = 0;
    LayoutResourceSet* resources;
    if (overrideResource) {
        resource = overrideResource;
        resources = context->overrideResources;
    } else {
        resources = context->resources;
    }
    pane->kind = (u8(resource->flags) >> WindowResource_KindShift) & 3;
    if (u8(resource->flags) & WindowResource_VertexColorForAll) pane->windowFlags = u8(pane->windowFlags) | Window_VertexColorForAll;
    if (u8(resource->flags) & WindowResource_OneMaterialForAll) pane->windowFlags = u8(pane->windowFlags) | Window_OneMaterialForAll;
    if (u8(resource->flags) & WindowResource_HideContent) pane->windowFlags = u8(pane->windowFlags) | Window_HideContent;
    auto* content = layoutAtOffset<WindowContentResource>(resource, resource->contentOffset);
    u32 coordinateCount = content->coordinateCount;
    if (coordinateCount > 3) coordinateCount = 3;
    pane->reserveCoordinates(coordinateCount);
    for (u32 side = 0; side < 4; ++side) pane->inflation[side] = s16(resource->inflation[side]);
    if (s16(pane->inflation[0]) || s16(pane->inflation[1]) || s16(pane->inflation[2]) || s16(pane->inflation[3]))
        pane->windowFlags = u8(pane->windowFlags) | Window_Inflated;
    switch (u8(pane->kind)) {
    case Window_Horizontal:
        pane->frameSizes[0] = resource->frameSizes[0];
        pane->frameSizes[1] = resource->frameSizes[1];
        pane->frameSizes[2] = 0;
        pane->frameSizes[3] = 0;
        pane->content()->flags = u8(pane->content()->flags) | 0x10;
        pane->content()->height = f32(u16(resource->frameSizes[2]));
        break;
    case Window_HorizontalNoContent: {
        /* the left frame takes what the right frame leaves of the width */
        f32 width = pane->content()->width;
        pane->frameSizes[0] = u16(gabi::ftoi(width)) - u16(resource->frameSizes[1]);
        pane->frameSizes[1] = resource->frameSizes[1];
        pane->frameSizes[2] = 0;
        pane->frameSizes[3] = 0;
        f32 height = f32(u16(resource->frameSizes[2]));
        pane->content()->flags = u8(pane->content()->flags) | 0x10;
        pane->content()->width = width;
        pane->content()->height = height;
        break;
    }
    default:
        for (u32 side = 0; side < 4; ++side) pane->frameSizes[side] = u16(resource->frameSizes[side]);
        break;
    }
    /* the original keeps the offset table address of the first read of the material table and
     * re-reads only the table base afterwards */
    ResourceNameTable* materials = resources->materials;
    be<u32>* materialOffsets = materials->offsets;
    for (u32 corner = 0; corner < 4; ++corner) pane->colors[corner].value = u32(content->colors[corner].value);
    if (coordinateCount && u8(pane->coordinates.capacity))
        windowSetCoordinates(&pane->coordinates, content->coordinates, coordinateCount);
    materials = resources->materials;
    void* contentMaterial = layoutAtOffset<void>(materials, materialOffsets[content->materialIndex]);
    void* storage = textPaneAlignedAllocate(kMaterialSize, 4);
    pane->material = storage ? textPaneCreateMaterial(storage, contentMaterial, resources) : nullptr;
    pane->frames = nullptr;
    pane->frameCount = 0;
    u32 frameCount = resource->frameCount;
    if (frameCount) {
        pane->reserveFrames(frameCount);
        auto* frameTable = layoutAtOffset<be<u32>>(resource, resource->frameTableOffset);
        for (u32 index = 0; index < u8(pane->frameCount); ++index) {
            auto* entry = layoutAtOffset<WindowFrameResource>(resource, frameTable[index]);
            pane->frames.get()[index].textureFlip = entry->textureFlip;
            materials = resources->materials;
            void* frameMaterial = layoutAtOffset<void>(materials, materialOffsets[entry->materialIndex]);
            void* frameStorage = textPaneAlignedAllocate(kMaterialSize, 4);
            LayoutMaterial* created = frameStorage ? textPaneCreateMaterial(frameStorage, frameMaterial, resources) : nullptr;
            pane->frames.get()[index].material = created;
        }
    }
    return pane;
}

/* 028793E4 */
void WindowFrame::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    LayoutMaterial* owned = material;
    if (owned && !(u8(owned->ownershipFlags) & Material_Shared)) destroyOwnedMaterial(owned);
    material = nullptr;
    if (flags & 1) textPaneDelete(this);
}

/* 02879474 */
void WindowPane::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    mVtable = gabi::ea(&sWindowPaneMethods);
    WindowFrame* ownedFrames = frames;
    u32 count = frameCount;
    if (ownedFrames) {
        for (u32 index = 0; index < count; ++index) ownedFrames[index].destroy(2);
        textPaneRelease(ownedFrames);
    }
    LayoutMaterial* owned = material;
    if (owned && !(u8(owned->ownershipFlags) & Material_Shared)) {
        destroyOwnedMaterial(owned);
        material = nullptr;
    }
    windowClearCoordinates(&coordinates);
    LayoutPane::destroy(0);
    if (flags & 1) textPaneDelete(this);
}

/* 02879550 */
void WindowPane::setColorElement(u32 index, u32 value) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    reinterpret_cast<be<u8>*>(colors)[index] = value;
}

/* 02879568 */
u32 WindowPane::materialCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return u8(u8(frameCount) + 1);   // the content material, then one per frame
}

/* 02879578 */
LayoutMaterial* WindowPane::materialAt(u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (index == 0) return material;
    s32 frameIndex = s32(index - 1);
    if (frameIndex < s32(u8(frameCount))) return frames.get()[frameIndex].material;
    return nullptr;
}

/* 028795B4 */
LayoutMaterial* WindowPane::findMaterial(const be<u8>* name, u32 recursive) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    LayoutMaterial* found = material;
    if (found && layoutLongNameEqual(found->name, name)) return found;
    u32 count = frameCount;
    WindowFrame* entries = frames;
    for (u32 index = 0; index < count; ++index) {
        found = entries[index].material;
        if (layoutLongNameEqual(found->name, name)) return found;
    }
    if (recursive) {
        auto* sentinel = &content()->children.root;
        for (nw::ut::HeapLink* child = sentinel->next; child != sentinel; child = child->next) {
            auto* pane = reinterpret_cast<LayoutPane*>(child);
            found = gabi::call_ptr<LayoutMaterial*>(paneMethods<PaneMethods>(pane)->findMaterial, pane, name, recursive);
            if (found) return found;
        }
    }
    return nullptr;
}

/* 0287B028 */
void WindowPane::drawContent(LayoutDrawContext* context, const PaneSize* origin, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    f32 left = insets->left;
    f32 top = insets->top;
    f32 x = gabi::fadds_ppc(origin->width, left);
    f32 y = gabi::fsubs_ppc(origin->height, top);
    f32 width = gabi::fsubs_ppc(gabi::fsubs_ppc(content()->width, left), insets->right);
    f32 height = gabi::fsubs_ppc(gabi::fsubs_ppc(content()->height, top), insets->bottom);
    u32 inflated = u8(windowFlags) & Window_Inflated;
    u32 coordinateCount = coordinates.count;
    if (inflated) {
        f32 inflateLeft = gabi::fmuls_ppc(f32(s16(inflation[0])), 0.0625f);
        f32 inflateTop = gabi::fmuls_ppc(f32(s16(inflation[2])), 0.0625f);
        f32 inflateHeight = gabi::fmadds(f32(s16(inflation[3])), 0.0625f, inflateTop);
        f32 inflateWidth = gabi::fmadds(f32(s16(inflation[1])), 0.0625f, inflateLeft);
        x = gabi::fsubs_ppc(x, inflateLeft);
        y = gabi::fadds_ppc(y, inflateTop);
        width = gabi::fadds_ppc(width, inflateWidth);
        height = gabi::fadds_ppc(height, inflateHeight);
    }
    gabi::FrameLocal<PaneSize> position(8);
    gabi::FrameLocal<PaneSize> size(0x10);
    position->width = x;
    position->height = y;
    size->width = width;
    size->height = height;
    windowDrawQuad(context, position.get(), size.get(), coordinateCount, coordinates.coordinates);
}

/* 0287D1C4 */
void WindowPane::drawHorizontalNoContent(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    f32 rightSize = f32(u16(frameSizes[1]));
    f32 leftSize = gabi::fsubs_ppc(content()->width, rightSize);
    gabi::FrameLocal<WindowInsets> insets(8);
    insets->bottom = 0.0f;
    insets->top = 0.0f;
    insets->right = rightSize;
    insets->left = leftSize;
    auto origin = originOffset();
    gabi::FrameLocal<PaneSize> position(0x18);
    storeWords(position.get(), origin);
    u32 count = frameCount;
    if (count == 1) drawHorizontalNoContentFrameOne(context, position.get(), frames, insets.get(), content()->inheritedAlpha);
    else if (count == 2 || count == 4) drawHorizontalNoContentFrameTwo(context, position.get(), frames, insets.get(), content()->inheritedAlpha);
}

/* Inlined helpers of the draw functions below: their stack objects live in the caller's frame at
 * the offsets the caller's disassembly uses (passed in as frame offsets). */
static u32 allColorsWhite(const WindowPane* pane) {
    for (u32 corner = 0; corner < 4; ++corner)
        if (u32(pane->colors[corner].value) != 0xFFFFFFFF) return 0;
    return u8(pane->content()->inheritedAlpha) == 255;
}
static void setupMaterial(WindowPane* pane, LayoutMaterial* material, LayoutDrawContext* context, u32 alpha, u32 white, u32 isContent, u32 frameOffset) {
    gabi::FrameLocal<PaneSize> translation(frameOffset);
    translation->width = pane->content()->matrix.m[0][3];
    translation->height = pane->content()->matrix.m[1][3];
    gabi::call_ptr<void>(materialMethods(material)->setup, material, context, alpha, white, isContent, translation.get());
}
static void uploadVertexColors(WindowPane* pane, LayoutDrawContext* context, u32 frameOffset) {
    u32 location = context->uniformLocation(Uniform_VertexColors);
    if (location == 0xFFFFFFFF) return;
    gabi::FrameLocal<UniformColors> colors(frameOffset);
    auto* channels = reinterpret_cast<const be<u8>*>(pane->colors);
    for (u32 index = 0; index < 16; ++index) colors->values[index] = f32(u8(channels[index]));
    windowSetVertexUniforms(location, 16, colors.get());
}
static void uploadWindowSize(WindowPane* pane, LayoutDrawContext* context, u32 frameOffset) {
    u32 location = context->uniformLocation(Uniform_WindowSize);
    gabi::FrameLocal<UniformVector> size(frameOffset);
    size->x = pane->content()->width;
    size->z = 0.0f;
    size->y = pane->content()->height;
    size->w = 0.0f;
    windowSetVertexUniforms(location, 4, size.get());
}
static void uploadFrameInsets(LayoutDrawContext* context, const WindowInsets* insets, u32 frameOffset) {
    u32 location = context->uniformLocation(Uniform_FrameInsets);
    gabi::FrameLocal<UniformVector> values(frameOffset);
    values->y = insets->right;
    values->x = insets->left;
    values->z = insets->top;
    values->w = insets->bottom;
    windowSetVertexUniforms(location, 4, values.get());
}
static void uploadFrameMode(LayoutDrawContext* context, u32 mode, u32 frameOffset) {
    u32 location = context->uniformLocation(Uniform_FrameMode);
    gabi::FrameLocal<UniformWords> values(frameOffset);
    values->z = 0;
    values->x = mode;
    values->w = 0;
    values->y = 0;
    windowSetVertexUniforms(location, 4, values.get());
}

/* 0287C358 */
void WindowPane::drawHorizontal(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<WindowInsets> insets(0x10);
    insets->left = f32(u16(frameSizes[0]));
    insets->right = f32(u16(frameSizes[1]));
    insets->top = f32(u16(frameSizes[2]));
    insets->bottom = f32(u16(frameSizes[3]));
    auto origin = originOffset();
    gabi::FrameLocal<PaneSize> position(0x20);
    storeWords(position.get(), origin);
    u32 count = frameCount;
    if (count == 1) drawHorizontalFrameOne(context, position.get(), frames, insets.get(), content()->inheritedAlpha);
    else if (count == 2 || count == 4) drawHorizontalFrameTwo(context, position.get(), frames, insets.get(), content()->inheritedAlpha);
    if (u8(windowFlags) & Window_HideContent) return;
    windowBindMaterial(material, context);
    LayoutMaterial* contentMaterial = material;
    gabi::FrameLocal<PaneSize> translation(0x28);
    translation->width = content()->matrix.m[0][3];
    translation->height = content()->matrix.m[1][3];
    windowSetMaterialTranslation(contentMaterial, context, translation.get());
    if (u8(windowFlags) & Window_VertexColorForAll) {
        uploadFrameMode(context, FrameMode_VertexColorContent, 0x70);
    } else {
        uploadVertexColors(this, context, 0x30);
        uploadFrameMode(context, 0, 0x80);
    }
    drawContent(context, position.get(), insets.get(), content()->inheritedAlpha);
}

/* 0287B180 */
void WindowPane::drawAround(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<WindowInsets> insets(0x10);
    insets->left = f32(u16(frameSizes[0]));
    insets->right = f32(u16(frameSizes[1]));
    insets->top = f32(u16(frameSizes[2]));
    insets->bottom = f32(u16(frameSizes[3]));
    auto origin = originOffset();
    gabi::FrameLocal<PaneSize> position(0x20);
    storeWords(position.get(), origin);
    switch (u8(frameCount)) {
    case 1: drawAroundFrameOne(context, position.get(), frames, insets.get(), content()->inheritedAlpha); break;
    case 4: drawAroundFrameFour(context, position.get(), frames, insets.get(), content()->inheritedAlpha); break;
    case 8: drawAroundFrameEight(context, position.get(), frames, insets.get(), content()->inheritedAlpha); break;
    }
    if (u8(windowFlags) & Window_HideContent) return;
    u32 white = allColorsWhite(this);
    LayoutMaterial* contentMaterial = material;
    setupMaterial(this, contentMaterial, context, white ? 255 : u8(content()->inheritedAlpha), white, 1, white ? 0x30 : 0x28);
    if (white) {
        uploadFrameMode(context, 0, 0xCC);
    } else if (u8(windowFlags) & Window_VertexColorForAll) {
        uploadWindowSize(this, context, 0x9C);
        uploadFrameInsets(context, insets.get(), 0x38);
        uploadVertexColors(this, context, 0x48);
        uploadFrameMode(context, FrameMode_VertexColorContent, 0xAC);
    } else {
        uploadVertexColors(this, context, 0x48);
        uploadFrameMode(context, 0, 0xBC);
    }
    drawContent(context, position.get(), insets.get(), content()->inheritedAlpha);
}

/* Frame mode words: a per-side rotation word or'ed with a per-texture-flip transform word, from
 * guarded function-local tables (copied from .data on first use). */
static FrameModeTable sHorizontalFlipDefaults;
static FrameModeTable sHorizontalSideDefaults;
static FrameModeTable sNoContentFlipDefaults;
static FrameModeTable sNoContentSideDefaults;
static FrameModeTable sHorizontalFlips;
static FrameModeTable sHorizontalSides;
static FrameModeTable sNoContentFlips;
static FrameModeTable sNoContentSides;
static TableGuard sHorizontalFlipGuard;
static TableGuard sHorizontalSideGuard;
static TableGuard sNoContentFlipGuard;
static TableGuard sNoContentSideGuard;
static FrameModeTable sAroundFlipDefaults;
static FrameModeTable sAroundSideDefaults;
static FrameModeTable sAroundFlips;
static FrameModeTable sAroundSides;
static TableGuard sAroundFlipGuard;
static TableGuard sAroundSideGuard;
static FrameModeTable sEightFlipDefaults;
static FrameModeTable sEightSideDefaults;
static FrameModeTable sEightFlips;
static FrameModeTable sEightSides;
static TableGuard sEightFlipGuard;
static TableGuard sEightSideGuard;
static UniformWords sWindowSizeDefaults;

static u32 frameMode(FrameModeTable* flips, TableGuard* flipGuard, const FrameModeTable* flipDefaults,
                     FrameModeTable* sides, TableGuard* sideGuard, const FrameModeTable* sideDefaults, u32 sideBytes,
                     u32 side, u32 textureFlip, u32 vertexColors) {
    if (!flipGuard->ready) {
        flipGuard->ready = 1;
        textPaneCopy(flips, flipDefaults, 0x18);
    }
    if (!sideGuard->ready) {
        sideGuard->ready = 1;
        textPaneCopy(sides, sideDefaults, sideBytes);
    }
    u32 mode = u32(sides->values[side]) | u32(flips->values[textureFlip]);
    if (!vertexColors) mode |= FrameMode_NoVertexColor;
    return mode;
}
static u32 horizontalFrameMode(u32 noContent, u32 side, u32 textureFlip, u32 vertexColors) {
    if (noContent)
        return frameMode(&sNoContentFlips, &sNoContentFlipGuard, &sNoContentFlipDefaults,
                         &sNoContentSides, &sNoContentSideGuard, &sNoContentSideDefaults, 0x20, side, textureFlip, vertexColors);
    return frameMode(&sHorizontalFlips, &sHorizontalFlipGuard, &sHorizontalFlipDefaults,
                     &sHorizontalSides, &sHorizontalSideGuard, &sHorizontalSideDefaults, 8, side, textureFlip, vertexColors);
}
static u32 aroundFrameMode(u32 eight, u32 side, u32 textureFlip, u32 vertexColors) {
    if (eight)
        return frameMode(&sEightFlips, &sEightFlipGuard, &sEightFlipDefaults,
                         &sEightSides, &sEightSideGuard, &sEightSideDefaults, 0x20, side, textureFlip, vertexColors);
    return frameMode(&sAroundFlips, &sAroundFlipGuard, &sAroundFlipDefaults,
                     &sAroundSides, &sAroundSideGuard, &sAroundSideDefaults, 0x10, side, textureFlip, vertexColors);
}

/* Shared body of the four horizontal frame drawers (with and without content, one or two frame
 * materials): a left and a right frame quad. */
static u32 prepareHorizontalFrames(WindowPane* pane, LayoutDrawContext* context, WindowFrame* frames, const WindowInsets* insets, u32 alpha, u32 one) {
    u32 white = allColorsWhite(pane);
    LayoutMaterial* first = frames[0].material;
    setupMaterial(pane, first, context, alpha, white, 0, one ? (white ? 0x28 : 0x20) : (white ? 0x30 : 0x28));
    if (!white) {
        if (one) uploadVertexColors(pane, context, 0x40);
        if (u8(pane->windowFlags) & Window_VertexColorForAll) {
            uploadWindowSize(pane, context, one ? 0x94 : 0x9C);
            uploadFrameInsets(context, insets, one ? 0x30 : 0x38);
            if (!one) uploadVertexColors(pane, context, 0x48);
        }
    }
    return u8(pane->windowFlags) & Window_VertexColorForAll;
}
static void drawHorizontalFrames(WindowPane* pane, LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames,
                                 const WindowInsets* insets, u32 alpha, u32 noContent, u32 one) {
    if (one) {
        LayoutMaterial* only = frames[0].material;
        if (!(u32(only->featureFlags) & kFrameMaterialsVisible)) return;
    }
    u32 vertexColors = prepareHorizontalFrames(pane, context, frames, insets, alpha, one);
    gabi::FrameLocal<PaneSize> leftPosition(one ? 0x18 : 0xCC);
    gabi::FrameLocal<PaneSize> leftSize(0x10);
    copyFloatBits(leftPosition->width, origin->width);
    copyFloatBits(leftPosition->height, origin->height);
    leftSize->width = insets->left;
    leftSize->height = pane->content()->height;
    u32 flip = one ? 0 : u8(frames[0].textureFlip);
    u32 mode = horizontalFrameMode(noContent, 0, flip, vertexColors);
    uploadFrameMode(context, mode, one ? 0xA4 : 0xAC);
    windowDrawRectangle(context, leftPosition.get(), leftSize.get());
    if (!one) windowBindMaterial(frames[1].material, context);
    f32 height = pane->content()->height;
    f32 rightX = gabi::fadds_ppc(origin->width, pane->content()->width);
    rightX = gabi::fsubs_ppc(rightX, insets->right);
    f32 y = origin->height;
    vertexColors = u8(pane->windowFlags) & Window_VertexColorForAll;
    gabi::FrameLocal<PaneSize> rightPosition(0x18);
    gabi::FrameLocal<PaneSize> rightSize(one ? 0x10 : 0x20);
    rightSize->height = height;
    rightSize->width = insets->right;
    rightPosition->height = y;
    rightPosition->width = rightX;
    flip = one ? 1 : u8(frames[1].textureFlip);
    mode = horizontalFrameMode(noContent, 1, flip, vertexColors);
    uploadFrameMode(context, mode, one ? 0xB4 : 0xBC);
    windowDrawNextRectangle(context, rightPosition.get(), rightSize.get());
}

/* 0287B8D4 */
void WindowPane::drawHorizontalFrameOne(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    drawHorizontalFrames(this, context, origin, frames, insets, alpha, 0, 1);
}

/* 0287BE14 */
void WindowPane::drawHorizontalFrameTwo(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    drawHorizontalFrames(this, context, origin, frames, insets, alpha, 0, 0);
}

/* 0287C740 */
void WindowPane::drawHorizontalNoContentFrameOne(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    drawHorizontalFrames(this, context, origin, frames, insets, alpha, 1, 1);
}

/* 0287CC80 */
void WindowPane::drawHorizontalNoContentFrameTwo(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    drawHorizontalFrames(this, context, origin, frames, insets, alpha, 1, 0);
}

static void uploadAroundSize(WindowPane* pane, LayoutDrawContext* context, u32 frameOffset) {
    u32 location = context->uniformLocation(Uniform_WindowSize);
    /* size through FPRs (a signalling NaN is quieted); z and w are the table's words */
    gabi::FrameLocal<UniformSize> values(frameOffset);
    values->width = pane->content()->width;
    values->z = u32(sWindowSizeDefaults.z);
    values->height = pane->content()->height;
    values->w = u32(sWindowSizeDefaults.w);
    windowSetVertexUniforms(location, 4, values.get());
}

/* 02879694 */
void WindowPane::drawAroundFrameOne(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    /* one material for all four sides: top, right, bottom and left quads around the content */
    LayoutMaterial* only = frames[0].material;
    if (!(u32(only->featureFlags) & kFrameMaterialsVisible)) return;
    u32 white = allColorsWhite(this);
    setupMaterial(this, only, context, alpha, white, 0, white ? 0x28 : 0x20);
    if (!white && (u8(windowFlags) & Window_VertexColorForAll)) {
        uploadWindowSize(this, context, 0x94);
        uploadFrameInsets(context, insets, 0x30);
        uploadVertexColors(this, context, 0x40);
    }
    u32 vertexColors = u8(windowFlags) & Window_VertexColorForAll;
    gabi::FrameLocal<PaneSize> position(0x18);
    gabi::FrameLocal<PaneSize> size(0x10);
    f32 top = insets->top;
    f32 topWidth = gabi::fsubs_ppc(content()->width, insets->right);
    size->height = top;
    copyFloatBits(position->width, origin->width);
    size->width = topWidth;
    copyFloatBits(position->height, origin->height);
    uploadFrameMode(context, aroundFrameMode(0, 0, 0, vertexColors), 0xA4);
    windowDrawRectangle(context, position.get(), size.get());
    f32 rightX = gabi::fsubs_ppc(gabi::fadds_ppc(origin->width, content()->width), insets->right);
    f32 rightHeight = gabi::fsubs_ppc(content()->height, insets->bottom);
    size->width = insets->right;
    position->width = rightX;
    size->height = rightHeight;
    position->height = origin->height;
    vertexColors = u8(windowFlags) & Window_VertexColorForAll;
    uploadFrameMode(context, aroundFrameMode(0, 1, 1, vertexColors), 0xB4);
    windowDrawRectangle(context, position.get(), size.get());
    f32 bottomY = gabi::fadds_ppc(gabi::fsubs_ppc(origin->height, content()->height), insets->bottom);
    f32 bottomX = gabi::fadds_ppc(origin->width, insets->left);
    f32 bottomWidth = gabi::fsubs_ppc(content()->width, insets->left);
    position->width = bottomX;
    size->height = insets->bottom;
    position->height = bottomY;
    size->width = bottomWidth;
    vertexColors = u8(windowFlags) & Window_VertexColorForAll;
    uploadFrameMode(context, aroundFrameMode(0, 3, 4, vertexColors), 0xC4);
    windowDrawRectangle(context, position.get(), size.get());
    f32 leftX = origin->width;
    f32 leftY = gabi::fsubs_ppc(origin->height, insets->top);
    f32 leftHeight = gabi::fsubs_ppc(content()->height, insets->top);
    position->width = leftX;
    size->width = insets->left;
    position->height = leftY;
    size->height = leftHeight;
    vertexColors = u8(windowFlags) & Window_VertexColorForAll;
    uploadFrameMode(context, aroundFrameMode(0, 2, 2, vertexColors), 0xD4);
    windowDrawRectangle(context, position.get(), size.get());
}

/* Material setup of one frame quad of the four- and eight-frame drawers; after the first drawn
 * frame ("next") a shared material only needs binding. */
static u32 prepareAroundFrame(WindowPane* pane, LayoutDrawContext* context, LayoutMaterial* material, const WindowInsets* insets, u32 alpha, u32 eight, u32 next) {
    u32 white = allColorsWhite(pane);
    if (next && (u8(pane->windowFlags) & Window_OneMaterialForAll)) {
        windowBindMaterial(material, context);
        return u8(pane->windowFlags) & Window_VertexColorForAll;
    }
    u32 translationOffset = eight ? (next ? (white ? 0x40 : 0x38) : (white ? 0x30 : 0x28))
                                  : (next ? (white ? 0x48 : 0x40) : (white ? 0x38 : 0x30));
    setupMaterial(pane, material, context, alpha, white, 0, translationOffset);
    if (!white && (u8(pane->windowFlags) & Window_VertexColorForAll)) {
        uploadAroundSize(pane, context, eight ? (next ? 0xCC : 0xBC) : (next ? 0xD4 : 0xC4));
        uploadFrameInsets(context, insets, eight ? (next ? 0x58 : 0x48) : (next ? 0x60 : 0x50));
        uploadVertexColors(pane, context, eight ? 0x68 : 0x70);
    }
    return u8(pane->windowFlags) & Window_VertexColorForAll;
}

/* 02879D60 */
void WindowPane::drawAroundFrameFour(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    /* corners: top-left, top-right, bottom-left, bottom-right, each frame stretched along one side */
    gabi::FrameLocal<WindowCorners> sizes(0xE4);
    gabi::FrameLocal<WindowCorners> positions(0x124);
    textPaneConstructArray(sizes.get(), 4, sizeof(PaneSize), gabi::guest_fn((const void*)windowCreatePoint));
    f32 height = content()->height;
    f32 bottom = insets->bottom;
    f32 left = insets->left;
    f32 y = origin->height;
    f32 right = insets->right;
    f32 top = insets->top;
    f32 width = content()->width;
    f32 x = origin->width;
    sizes->values[1].height = gabi::fsubs_ppc(height, bottom);
    sizes->values[0].height = top;
    sizes->values[2].width = left;
    sizes->values[0].width = gabi::fsubs_ppc(width, right);
    positions->values[0].height = y;
    positions->values[0].width = x;
    sizes->values[1].width = right;
    positions->values[1].width = gabi::fsubs_ppc(gabi::fadds_ppc(x, width), right);
    positions->values[1].height = y;
    positions->values[2].height = gabi::fsubs_ppc(y, top);
    sizes->values[2].height = gabi::fsubs_ppc(height, top);
    positions->values[2].width = x;
    positions->values[3].width = gabi::fadds_ppc(x, left);
    positions->values[3].height = gabi::fadds_ppc(gabi::fsubs_ppc(y, height), bottom);
    sizes->values[3].width = gabi::fsubs_ppc(width, left);
    sizes->values[3].height = bottom;
    u32 next = 0;
    for (u32 index = 0; index < 4; ++index) {
        LayoutMaterial* material = frames[index].material;
        if (!material || !(u32(material->featureFlags) & kFrameMaterialsVisible)) continue;
        u32 vertexColors = prepareAroundFrame(this, context, material, insets, alpha, 0, next);
        u32 flip = frames[index].textureFlip;
        uploadFrameMode(context, aroundFrameMode(0, index, flip, vertexColors), next ? 0x114 : 0x104);
        if (next) {
            windowDrawNextRectangle(context, &positions->values[index], &sizes->values[index]);
        } else {
            windowDrawRectangle(context, &positions->values[index], &sizes->values[index]);
            next = 1;
        }
    }
}

/* 0287A6B8 */
void WindowPane::drawAroundFrameEight(LayoutDrawContext* context, const PaneSize* origin, WindowFrame* frames, const WindowInsets* insets, u32 alpha) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    /* four corners, then the left, right, top and bottom sides between them */
    gabi::FrameLocal<WindowRectangles> rectangles(0xDC);
    f32 right = insets->right;
    f32 width = content()->width;
    f32 left = insets->left;
    f32 x = origin->width;
    f32 top = insets->top;
    f32 height = content()->height;
    f32 y = origin->height;
    f32 bottom = insets->bottom;
    f32 rightX = gabi::fsubs_ppc(gabi::fadds_ppc(x, width), right);
    f32 bottomY = gabi::fadds_ppc(gabi::fsubs_ppc(y, height), bottom);
    f32 middleHeight = gabi::fsubs_ppc(gabi::fsubs_ppc(height, top), bottom);
    f32 middleY = gabi::fsubs_ppc(y, top);
    f32 middleX = gabi::fadds_ppc(x, left);
    f32 middleWidth = gabi::fsubs_ppc(gabi::fsubs_ppc(width, left), right);
    const f32 layout[8][4] = {
        {x, y, left, top}, {rightX, y, right, top}, {x, bottomY, left, bottom}, {rightX, bottomY, right, bottom},
        {x, middleY, left, middleHeight}, {rightX, middleY, right, middleHeight},
        {middleX, y, middleWidth, top}, {middleX, bottomY, middleWidth, bottom},
    };
    for (u32 index = 0; index < 8; ++index) {
        rectangles->values[index].position.width = layout[index][0];
        rectangles->values[index].position.height = layout[index][1];
        rectangles->values[index].size.width = layout[index][2];
        rectangles->values[index].size.height = layout[index][3];
    }
    u32 next = 0;
    for (u32 index = 0; index < 8; ++index) {
        LayoutMaterial* material = frames[index].material;
        if (!(u32(material->featureFlags) & kFrameMaterialsVisible)) continue;
        u32 vertexColors = prepareAroundFrame(this, context, material, insets, alpha, 1, next);
        u32 flip = frames[index].textureFlip;
        uploadFrameMode(context, aroundFrameMode(1, index, flip, vertexColors), next ? 0x16C : 0x15C);
        WindowRectangle* rectangle = &rectangles->values[index];
        if (next) {
            windowDrawNextRectangle(context, &rectangle->position, &rectangle->size);
        } else {
            windowDrawRectangle(context, &rectangle->position, &rectangle->size);
            next = 1;
        }
    }
}

/* WWHD glue
data sWindowPaneMethods 10180874
data sHorizontalFlipDefaults 101807F8
data sHorizontalSideDefaults 10180790
data sNoContentFlipDefaults 10180830
data sNoContentSideDefaults 10180810
data sHorizontalFlips 101FE158
data sHorizontalSides 101FE170
data sNoContentFlips 101FE178
data sNoContentSides 101FE190
data sHorizontalFlipGuard 101FD8B4
data sHorizontalSideGuard 101FD8B0
data sNoContentFlipGuard 101FD8BC
data sNoContentSideGuard 101FD8B8
data sAroundFlipDefaults 101807E0
data sAroundSideDefaults 101807D0
data sAroundFlips 101FE0F8
data sAroundSides 101FE110
data sAroundFlipGuard 101FD8AC
data sAroundSideGuard 101FD8A8
data sEightFlipDefaults 101807B8
data sEightSideDefaults 10180798
data sEightFlips 101FE140
data sEightSides 101FE120
data sEightFlipGuard 101FD8A4
data sEightSideGuard 101FD8A0
data sWindowSizeDefaults 10180848
ext 02874024 u32 nw::lyt::LayoutDrawContext::uniformLocation(u32)
ext C0006A40 void windowSetVertexUniforms(u32,u32,const void*)
ext 02880CE8 void windowCreateCoordinates(nw::lyt::TextureCoordinateStorage*)
ext 02880D2C void windowClearCoordinates(nw::lyt::TextureCoordinateStorage*)
ext 02880D84 void windowReserveCoordinates(nw::lyt::TextureCoordinateStorage*,u32)
ext 02880E24 void windowSetCoordinates(nw::lyt::TextureCoordinateStorage*,const nw::lyt::LayoutTextureCoordinates*,u32)
ext 02880F1C void windowDrawQuad(nw::lyt::LayoutDrawContext*,const nw::lyt::PaneSize*,const nw::lyt::PaneSize*,u32,nw::lyt::LayoutTextureCoordinates*)
ext 02880E90 void windowDrawRectangle(nw::lyt::LayoutDrawContext*,const nw::lyt::PaneSize*,const nw::lyt::PaneSize*)
ext 028810B0 void windowDrawNextRectangle(nw::lyt::LayoutDrawContext*,const nw::lyt::PaneSize*,const nw::lyt::PaneSize*)
ext 02882E74 void windowBindMaterial(nw::lyt::LayoutMaterial*,nw::lyt::LayoutDrawContext*)
ext 0288285C void windowSetMaterialTranslation(nw::lyt::LayoutMaterial*,nw::lyt::LayoutDrawContext*,const nw::lyt::PaneSize*)
ext 0287D394 nw::lyt::ColorWord* windowCreateColor(nw::lyt::ColorWord*) callback
ext 0287D3D0 nw::lyt::PaneSize* windowCreatePoint(nw::lyt::PaneSize*) callback
*/
