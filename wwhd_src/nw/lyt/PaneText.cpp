/* nw::lyt text box pane: UTF-16 text storage, font and alignment setup for the text writer, glyph
 * batch updates and drawing through the quad renderer; plus the out-of-line copies of the pane
 * base class's inline virtuals that GHS emitted in this translation unit.
 * Written from WWHD's disassembly; layouts from its constructors and method tables.
 * format facts: BFLYT txt1 block (public format documentation; facts only, no text or code copied) */
#include "nw/lyt/PaneText.h"
using namespace nw::lyt;

enum PaneStateFlag : u32 { Pane_Visible = 1, Pane_InfluencedAlpha = 2 };
static constexpr u32 kRenderDeviceRenderer = 8;   // QuadRenderer offset inside the render device
static constexpr u32 kMaterialSize = 0x3C;         // sizeof(LayoutMaterial)

static u8 sTextPaneMethods;
static RuntimeTypeGuard sTextIdentityGuard;
static Mtx34 sTextIdentity;

/* A float the original loads and stores again with no other memory access in between keeps its
 * exact bits in the recompiled reference (a signalling NaN is not quieted); a float that waits in
 * an FPR across another load or store is quieted like any f32 value. */
static inline void copyFloatBits(be<f32>& destination, const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination) = (u32) reinterpret_cast<const be<u32>&>(source);
}
/* Two floats returned in r3:r4 (Pair32), as the original passes them. */
static inline u32 floatWord(f32 value) { u32 word; __builtin_memcpy(&word, &value, 4); return word; }
static inline f32 wordFloat(u32 word) { f32 value; __builtin_memcpy(&value, &word, 4); return value; }
static inline nw::ut::QuadRenderer* renderDeviceRenderer(LayoutDrawContext* context) {
    return layoutAtOffset<nw::ut::QuadRenderer>(context->device.get(), kRenderDeviceRenderer);
}
static inline FontProviderMethods* fontProviderMethods(ResourceProvider* provider) {
    return gabi::at<FontProviderMethods>(gabi::ea(provider->methods.get()));
}
static inline void setColor(be<f32>* color, f32 red, f32 green, f32 blue, f32 alpha) {
    color[0] = red; color[1] = green; color[2] = blue; color[3] = alpha;
}

/* 02877100 */
void LayoutPaneInlines::draw(void* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 state = content()->flags;
    if (!(state & Pane_Visible)) return;
    if (content()->inheritedAlpha != 0) {
        gabi::call_ptr<void>(paneMethods<PaneMethods>(this)->drawSelf, this, context);
        state = content()->flags;
    }
    if ((state & Pane_InfluencedAlpha) && content()->alpha == 0) return;
    auto* sentinel = &content()->children.root;
    for (nw::ut::HeapLink* child = sentinel->next; child != sentinel; child = child->next) {
        auto* pane = reinterpret_cast<LayoutPane*>(child);
        gabi::call_ptr<void>(paneMethods<PaneMethods>(pane)->draw, pane, context);
    }
}

/* 028771B4 */
void LayoutPaneInlines::drawSelf(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
}

/* 028771B8 */
void LayoutPaneInlines::loadMatrix(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* source = reinterpret_cast<be<u32>*>(&content()->matrix);
    auto* destination = reinterpret_cast<be<u32>*>(&context->affineB);
    for (u32 word = 0; word < 12; ++word) destination[word] = u32(source[word]);
    context->affineUploaded = 0;
}

/* 028771F8 */
LayoutPane* LayoutPaneInlines::findPaneByName(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (layoutShortNameEqual(content()->name, name)) return this;
    auto* sentinel = &content()->children.root;
    for (nw::ut::HeapLink* child = sentinel->next; child != sentinel; child = child->next) {
        auto* pane = reinterpret_cast<LayoutPane*>(child);
        auto* found = gabi::call_ptr<LayoutPane*>(paneMethods<PaneMethods>(pane)->findPaneByName, pane, name);
        if (found) return found;
    }
    return nullptr;
}

/* 02877290 */
LayoutMaterial* LayoutPaneInlines::findMaterialByName(const be<u8>* name) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 count = gabi::call_ptr<u32>(paneMethods<PaneMethods>(this)->materialCount, this);
    for (u32 index = 0; index < count; ++index) {
        auto* material = gabi::call_ptr<LayoutMaterial*>(paneMethods<PaneMethods>(this)->materialAt, this, index);
        if (material && layoutLongNameEqual(material->name, name)) return material;
    }
    auto* sentinel = &content()->children.root;
    for (nw::ut::HeapLink* child = sentinel->next; child != sentinel; child = child->next) {
        auto* pane = reinterpret_cast<LayoutPane*>(child);
        auto* found = gabi::call_ptr<LayoutMaterial*>(paneMethods<PaneMethods>(pane)->findMaterialByName, pane, name);
        if (found) return found;
    }
    return nullptr;
}

/* 02877358 */
PaneUpdateContext* PaneUpdateContext::create(PaneUpdateContext* context, LayoutDrawContext* drawContext) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!context) context = static_cast<PaneUpdateContext*>(textPaneAllocate(sizeof(PaneUpdateContext)));
    if (context) {
        context->renderer = renderDeviceRenderer(drawContext);
        context->rootMatrix = &drawContext->affineA;
        copyFloatBits(context->scaleX, drawContext->scaleX);
        copyFloatBits(context->scaleY, drawContext->scaleY);
        context->alpha = 1.0f;
        context->scaleEnabled = (u8(drawContext->initializedByte) & Draw_ScaleEnabled) != 0;
        context->showInvisible = (u8(drawContext->initializedByte) & Draw_ShowInvisible) != 0;
        context->showTransparent = (u8(drawContext->initializedByte) & Draw_ShowTransparent) != 0;
        context->alphaEnabled = 0;
    }
    return context;
}

/* 028773F4 */
RuntimeTypeNode* nw::lyt::paneRuntimeType() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!sPaneTypeGuard.ready) {
        sPaneTypeGuard.ready = 1;
        sPaneType.parent = nullptr;
    }
    return &sPaneType;
}

/* 02877420 */
u32 LayoutPaneInlines::materialCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return 0;
}

/* 02877428 */
void TextPane::initializeText() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    bufferLength = 0;
    fontHeight = 0.0f;
    font = nullptr;
    fontWidth = 0.0f;
    textId = nullptr;
    position = 0;
    text = nullptr;
    length = 0;
    textFlags = u8(textFlags);   // inlined dirty-flag update with "unchanged": rewrites the byte as is
    shadowColors[1].value = 0;
    batch = nullptr;
    characterSpace = 0.0f;
    shadowOffset[0] = 0.0f;
    shadowOffset[1] = 0.0f;
    shadowColors[0].value = 0x000000FF;
    shadowColors[1].value = 0x000000FF;
    lineSpace = 0.0f;
    shadowItalicRatio = 0.0f;
    tagProcessor = nullptr;
    italicRatio = 0.0f;
    shadowScale[1] = 1.0f;
    shadowScale[0] = 1.0f;
    textPaneFill(&textFlags, 0, 1);
}

/* 028774E8 */
void TextPane::reserveText(u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!count) return;
    u32 quadCount = count;
    if (u8(textFlags) & TextPane_Shadow) quadCount *= 2;   // the shadow doubles every glyph quad
    u32 requested = count + 1;
    if (requested <= u16(bufferLength)) return;
    gabi::call_ptr<void>(paneMethods<PaneMethods>(this)->releaseText, this);
    u32 batchSize = GlyphWriter::batchBytes(quadCount);
    u32 vertexSize = GlyphWriter::vertexBytes(quadCount);
    auto* storage = static_cast<be<u16>*>(textPaneAlignedAllocate(requested * 2, 4));
    if (storage) {
        be<u16>* character = storage;
        for (u32 index = 0; index < requested; ++index) {
            if (character) *character = 0;
            ++character;
        }
    }
    void* batchStorage = textPaneAlignedAllocate(batchSize, 4);
    vertices = textPaneAlignedAllocate(vertexSize, 0x40);
    if (storage && batchStorage && vertices) {
        text = storage;
        void* vertexStorage = vertices;
        bufferLength = requested;
        batch = GlyphWriter::initializeBatch(static_cast<GlyphBatch_l*>(batchStorage), gabi::ea(vertexStorage), quadCount);
        return;
    }
    if (storage) textPaneRelease(storage);
    if (batchStorage) textPaneRelease(batchStorage);
    if (vertices) textPaneRelease(vertices);
}

/* 02877668 */
void TextPane::setFont(FontInterface* replacement) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (replacement && gabi::call_ptr<u32>(replacement->vtable->encoding, replacement) != 1) return;   // UTF-16 fonts only
    u32 changed = font.get() != replacement;
    textFlags = u8(textFlags) | (changed << 5);
    if (!changed) return;
    font = replacement;
    f32 width = 0.0f;
    f32 height = 0.0f;
    if (replacement) {
        width = f32(gabi::call_ptr<s32>(replacement->vtable->width, replacement));
        FontInterface* current = font;
        height = f32(gabi::call_ptr<s32>(current->vtable->height, current));
    }
    u32 sizeChanged = !(f32(fontWidth) == width && f32(fontHeight) == height);
    textFlags = u8(textFlags) | (sizeChanged << 5);
    if (sizeChanged) {
        fontHeight = height;
        fontWidth = width;
    }
}

/* 02877834 */
u32 TextPane::textBufferLength() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 capacity = bufferLength;
    return capacity ? u16(capacity - 1) : 0;   // one slot holds the terminator
}

/* 02877850 */
u32 TextPane::copyText(const be<u16>* source, u32 offset, u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!font || !text) return 0;
    be<u16>* storage = text;
    u32 capacity = textBufferLength();
    if (offset >= capacity) return 0;
    u32 copied = capacity - offset;
    if (count < copied) copied = count;
    textPaneCopy(storage + offset, source, copied * 2);
    storage = text;
    u16 end = *storage ? u16(offset + copied) : 0;
    length = end;
    storage[end] = 0;
    textFlags = u8(textFlags) | TextPane_Dirty;
    return u16(copied);
}

/* 02877930 */
u32 TextPane::setText(const be<u16>* source, u32 offset) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 count = 0;
    for (const be<u16>* character = source; *character; ++character) ++count;
    return copyText(source, offset, count);
}

/* 02877958 */
u32 TextPane::setTextCount(const be<u16>* source, u32 offset, u32 count) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return copyText(source, offset, count);
}

/* 02877F80 */
void TextPane::releaseText() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!text) return;
    textPaneRelease(batch);
    textPaneRelease(vertices);
    if (text) textPaneRelease(text);
    length = 0;
    batch = nullptr;
    bufferLength = 0;
    text = nullptr;
}

/* 02877FE8 */
void TextPane::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    LayoutMaterial* owned = material;
    mVtable = gabi::ea(&sTextPaneMethods);
    if (owned && !(u8(owned->ownershipFlags) & Material_Shared)) {
        gabi::call_ptr<void>(materialMethods(owned)->destroy, owned, u32(2));
        textPaneRelease(owned);
        material = nullptr;
    }
    releaseText();
    LayoutPane::destroy(0);
    if (flags & 1) textPaneDelete(this);
}

/* 02878098 */
FontInterface* TextPane::getFont() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return font;
}

/* 028780A0 */
void TextPane::setColorElement(u32 index, u32 value) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    /* index: vertex-colour element corner*4 + channel; the two top corners share the top colour,
     * the two bottom corners the bottom colour */
    auto* channel = reinterpret_cast<be<u8>*>(colors) + ((index >> 1) & ~3u) + (index & 3);
    u32 changed = u32(*channel) != value;
    textFlags = u8(textFlags) | (changed << 5);
    *channel = value;
}

/* 02878480 */
u32 TextPane::materialCount() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return material ? 1 : 0;
}

/* 02878490 */
LayoutMaterial* TextPane::materialAt(u32 index) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    return index == 0 ? material.get() : nullptr;
}

/* 02878CD4 */
void TextPane::loadMatrix(LayoutDrawContext* context) {   // text uses its own matrix (textMatrix)
    auto frame = gabi::AutoFrame(gabi::current_function());
}

/* 02878CD8 */
void TextPane::setupAlignment(TextLayoutWriter* writer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 horizontal = u8(position) & 3;
    u32 alignment = (u8(textFlags) >> TextPane_AlignmentShift) & 3;
    u32 style;
    if (alignment) style = alignment - 1;   // explicit line alignment
    else style = horizontal == TextPosition_Center ? 1 : horizontal == TextPosition_Far ? 2 : 0;   // follow the position
    if (horizontal == TextPosition_Center) style |= 0x10;
    else if (horizontal == TextPosition_Far) style |= 0x20;
    u32 vertical = (u8(position) >> 2) & 3;
    if (vertical == TextPosition_Center) style |= 0x100;
    else if (vertical == TextPosition_Far) style |= 0x200;
    writer->styleFlags = style;
}

/* 02878E44 */
ColorWord* nw::lyt::createTextColor(ColorWord* color) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!color) color = static_cast<ColorWord*>(textPaneAllocate(sizeof(ColorWord)));
    if (color) color->value = 0xFFFFFFFF;
    return color;
}

/* 02878E80 */
RuntimeTypeNode* TextPane::runtimeType() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!sTextPaneTypeGuard.ready) {
        sTextPaneTypeGuard.ready = 1;
        if (!sPaneTypeGuard.ready) {
            sPaneTypeGuard.ready = 1;
            sPaneType.parent = nullptr;
        }
        sTextPaneType.parent = &sPaneType;
    }
    return &sTextPaneType;
}

/* 028780DC */
Pair32 TextPane::alignmentOffset(const PaneSize* size, u32 round) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    f32 x = 0.0f;
    f32 y = 0.0f;
    u32 codes = position;
    switch (codes & 3) {
    case TextPosition_Center:
        x = gabi::fmuls_ppc(size->width, 0.5f);
        if (round) {
            x = textPaneFloor(x);
            codes = position;
        }
        break;
    case TextPosition_Far:
        x = size->width;
        break;
    }
    switch ((codes >> 2) & 3) {
    case TextPosition_Center:
        y = gabi::fmuls_ppc(size->height, 0.5f);
        if (round) y = textPaneFloor(y);
        break;
    case TextPosition_Far:
        y = size->height;
        break;
    }
    return Pair32{floatWord(x), floatWord(y)};
}

/* 0287820C */
void TextPane::applyFont(TextLayoutWriter* writer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    writer->font = font;
    if (font) {
        writer->setSize(fontWidth, fontHeight);
        auto* settings = reinterpret_cast<TextMeasureSettings*>(writer->reserved);
        copyFloatBits(settings->characterSpacing, characterSpace);
        copyFloatBits(settings->lineSpacing, lineSpace);
        if (u8(textFlags) & TextPane_WidthLimit) copyFloatBits(settings->widthLimit, content()->width);
        else settings->widthLimit = 0x1.fffffep+127f;
    }
    if (tagProcessor) writer->tagProcessor = tagProcessor;
}

/* 028784A8 */
void TextPane::textMatrix(Mtx34* destination) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (&content()->matrix != destination) {
        auto* source = reinterpret_cast<be<u32>*>(&content()->matrix);
        auto* output = reinterpret_cast<be<u32>*>(destination);
        for (u32 word = 0; word < 12; ++word) output[word] = u32(source[word]);
    }
    auto origin = originOffset();
    auto alignment = alignmentOffset(size(), 0);
    f32 horizontal = gabi::fadds_ppc(wordFloat(origin.r3), wordFloat(alignment.r3));
    f32 vertical = gabi::fsubs_ppc(wordFloat(origin.r4), wordFloat(alignment.r4));
    /* translate by (horizontal, vertical) in the pane's axes, then flip the Y axis for text */
    f32 axisY0 = destination->m[0][1];
    f32 axisY1 = destination->m[1][1];
    f32 axisY2 = destination->m[2][1];
    f32 offset0 = gabi::fmuls_ppc(axisY0, vertical);
    f32 offset1 = gabi::fmuls_ppc(axisY1, vertical);
    offset0 = gabi::fmadds(destination->m[0][0], horizontal, offset0);
    offset1 = gabi::fmadds(destination->m[1][0], horizontal, offset1);
    f32 translate0 = gabi::fadds_ppc(destination->m[0][3], offset0);
    f32 translate1 = gabi::fadds_ppc(destination->m[1][3], offset1);
    f32 offset2 = gabi::fmuls_ppc(axisY2, vertical);
    offset2 = gabi::fmadds(destination->m[2][0], horizontal, offset2);
    destination->m[0][3] = translate0;
    f32 translate2 = gabi::fadds_ppc(destination->m[2][3], offset2);
    destination->m[1][3] = translate1;
    destination->m[0][1] = -axisY0;
    destination->m[2][3] = translate2;
    destination->m[1][1] = -axisY1;
    destination->m[2][1] = -axisY2;
}

/* 02878D9C */
void TextPane::setupWriter(TextLayoutWriter* writer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    writer->x = 0.0f;
    writer->y = 0.0f;
    applyFont(writer);
    setupAlignment(writer);
    u32 top = colors[0].value;
    u32 bottom = colors[1].value;
    writer->colors[0].value = top;
    writer->colors[1].value = bottom;
    copyFloatBits(writer->bearing, italicRatio);
}

/* 0287795C */
TextPane* TextPane::createFromResource(TextPane* pane, TextBoxResource* resource, TextBoxResource* overrideResource, PaneBuildContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<TextPane*>(textPaneAllocate(sizeof(TextPane)));
    if (!pane) return pane;
    LayoutPane::createFromResource(pane, resource, context);
    pane->mVtable = gabi::ea(&sTextPaneMethods);
    textPaneConstructArray(pane->colors, 2, sizeof(ColorWord), gabi::guest_fn((const void*)createTextColor));
    pane->shadowColors[1].value = 0xFFFFFFFF;
    pane->shadowColors[0].value = 0xFFFFFFFF;
    pane->fontHeight = 0.0f;
    pane->fontWidth = 0.0f;
    /* An override block replaces the text; with KeepOwnResource the rest still comes from the
     * pane's own block and resources. */
    LayoutResourceSet* searchResources;
    LayoutResourceSet* resources;
    if (overrideResource) {
        searchResources = context->overrideResources;
        if (u16(context->flags) & PaneBuild_KeepOwnResource) {
            resources = context->resources;
        } else {
            resource = overrideResource;
            resources = searchResources;
        }
    } else {
        resources = context->resources;
        searchResources = resources;
        overrideResource = resource;
    }
    pane->initializeText();
    u32 bufferLength = resource->bufferBytes >> 1;
    u32 shadow = u8(resource->flags) & TextBoxResource_Shadow;
    u32 textLength = overrideResource->textBytes >> 1;
    if (bufferLength) --bufferLength;   // without the terminator
    if (textLength) --textLength;
    for (u32 index = 0; index < 2; ++index) pane->colors[index].value = u32(resource->colors[index].value);
    copyFloatBits(pane->fontWidth, resource->fontWidth);
    copyFloatBits(pane->fontHeight, resource->fontHeight);
    pane->position = resource->position;
    u32 flags = (u8(pane->textFlags) & 0x3F) | (u8(resource->alignment) << TextPane_AlignmentShift);
    pane->textFlags = flags;
    copyFloatBits(pane->characterSpace, resource->characterSpace);
    copyFloatBits(pane->lineSpace, resource->lineSpace);
    pane->textFlags = (flags & ~u32(TextPane_Shadow)) | (shadow << 4);
    pane->italicRatio = f32(resource->italicRatio);
    /* both values of a pair are loaded before either is stored (FPR copies: a signalling NaN is quieted) */
    f32 shadowX = resource->shadowOffset[0];
    f32 shadowY = resource->shadowOffset[1];
    pane->shadowOffset[0] = shadowX;
    pane->shadowOffset[1] = shadowY;
    f32 shadowScaleX = resource->shadowScale[0];
    f32 shadowScaleY = resource->shadowScale[1];
    pane->shadowScale[0] = shadowScaleX;
    pane->shadowScale[1] = shadowScaleY;
    pane->shadowColors[0].value = u32(resource->shadowColors[0].value);
    pane->shadowColors[1].value = u32(resource->shadowColors[1].value);
    copyFloatBits(pane->shadowItalicRatio, resource->shadowItalicRatio);
    /* width limit always on; the border bits of the resource map to the pane's draw bits */
    u32 resourceFlags = resource->flags;
    u32 invisibleBorder = (resourceFlags & TextBoxResource_InvisibleBorder) != 0;
    u32 doubleDrawnBorder = (resourceFlags & TextBoxResource_DoubleDrawnBorder) != 0;
    u32 drawBits = TextPane_WidthLimit | (doubleDrawnBorder ? TextPane_DoubleDrawnBorder : 0) | (invisibleBorder ? TextPane_InvisibleBorder : 0);
    pane->textFlags = (u8(pane->textFlags) & ~0xEu) | drawBits;
    ResourceNameTable* fonts = resources->fonts;
    ResourceProvider* provider = resources->provider;
    const be<u8>* fontName = fonts->nameAt(resource->fontIndex);
    pane->font = gabi::call_ptr<FontInterface*>(fontProviderMethods(provider)->findFont, provider, fontName);
    ResourceNameTable* materials = resources->materials;
    void* materialResource = materials->blockEntry(resource->materialIndex);
    void* materialStorage = textPaneAlignedAllocate(kMaterialSize, 4);
    pane->material = materialStorage ? textPaneCreateMaterial(materialStorage, materialResource, resources) : nullptr;
    u32 textIdOffset = overrideResource->textIdOffset;
    if (textIdOffset) pane->textId = layoutAtOffset<const be<u16>>(overrideResource, textIdOffset);
    TextSearcher* searcher = context->textSearcher;
    if (searcher) {
        gabi::FrameLocal<TextSearchResult> result(0x14);
        result->text = nullptr;
        result->reserved = 0;
        result->length = 0;
        result->bufferLength = 0;
        result->maximumLength = -1;
        if (u8(resource->flags) & TextBoxResource_RestrictLength) result->maximumLength = bufferLength;
        gabi::call_ptr<void>(searcher->methods->search, searcher, result.get(), pane->textId.get(),
                             context->searchUserData.get(), pane, searchResources->userData.get());
        if (result->text) {
            u32 count = result->length;
            if (!count) {
                for (const be<u16>* character = result->text; *character; ++character) ++count;
                count = u16(count);
            }
            u32 requested = result->bufferLength;
            pane->reserveText(u16(requested ? requested : count));
            if (pane->text) pane->setTextCount(result->text, 0, count);
            return pane;
        }
    }
    if (textLength > bufferLength) bufferLength = textLength;
    pane->reserveText(u16(bufferLength));
    if (textLength && pane->text) {
        auto* source = layoutAtOffset<const be<u16>>(overrideResource, overrideResource->textOffset);
        pane->setTextCount(source, 0, u16(textLength));
    }
    return pane;
}

/* 02877DB8 */
TextPane* TextPane::copyFrom(TextPane* pane, TextPane* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!pane) pane = static_cast<TextPane*>(textPaneAllocate(sizeof(TextPane)));
    if (!pane) return pane;
    LayoutPane::copyFrom(pane, source);
    pane->mVtable = gabi::ea(&sTextPaneMethods);
    pane->text = nullptr;
    pane->textId = source->textId;
    textPaneConstructArray(pane->colors, 2, sizeof(ColorWord), gabi::guest_fn((const void*)createTextColor));
    pane->font = source->font;
    copyFloatBits(pane->fontWidth, source->fontWidth);
    copyFloatBits(pane->fontHeight, source->fontHeight);
    copyFloatBits(pane->lineSpace, source->lineSpace);
    copyFloatBits(pane->characterSpace, source->characterSpace);
    pane->tagProcessor = source->tagProcessor;
    pane->length = 0;
    pane->bufferLength = 0;
    pane->position = source->position;
    copyFloatBits(pane->italicRatio, source->italicRatio);
    for (u32 index = 0; index < 2; ++index) copyFloatBits(pane->shadowOffset[index], source->shadowOffset[index]);
    for (u32 index = 0; index < 2; ++index) copyFloatBits(pane->shadowScale[index], source->shadowScale[index]);
    pane->shadowColors[0].value = u32(source->shadowColors[0].value);
    pane->shadowColors[1].value = u32(source->shadowColors[1].value);
    copyFloatBits(pane->shadowItalicRatio, source->shadowItalicRatio);
    pane->material = nullptr;
    pane->textFlags = source->textFlags;
    pane->batch = nullptr;
    pane->vertices = nullptr;
    for (u32 index = 0; index < 2; ++index) pane->colors[index].value = u32(source->colors[index].value);
    u32 capacity = source->bufferLength;
    if (capacity) {
        pane->reserveText(capacity);
        pane->setTextCount(source->text, 0, source->length);
    }
    LayoutMaterial* material = source->material;
    void* storage = textPaneAlignedAllocate(kMaterialSize, 4);
    pane->material = storage ? textPaneCopyMaterial(storage, material) : nullptr;
    return pane;
}

/* 028782A8 */
void TextPane::textBounds(TextBounds* output) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!font) {
        if (!output) output = static_cast<TextBounds*>(textPaneAllocate(sizeof(TextBounds)));
        if (output) {
            output->top = 0.0f;
            output->right = 0.0f;
            output->left = 0.0f;
            output->bottom = 0.0f;
        }
        return;
    }
    gabi::FrameLocal<TextLayoutWriter> writer(0x38);
    TextLayoutWriter::create(writer.get());
    writer->x = 0.0f;
    writer->y = 0.0f;
    applyFont(writer.get());
    gabi::FrameLocal<TextBounds> bounds(8);
    bounds->bottom = 0.0f;
    bounds->left = 0.0f;
    bounds->top = 0.0f;
    bounds->right = 0.0f;
    writer->measureBounds(bounds.get(), text, length);
    gabi::FrameLocal<PaneSize> extent(0x18);
    extent->width = gabi::fsubs_ppc(bounds->right, bounds->left);
    extent->height = gabi::fsubs_ppc(bounds->bottom, bounds->top);
    auto origin = originOffset();
    auto paneOffset = alignmentOffset(size(), 0);
    auto textOffset = alignmentOffset(extent.get(), 1);
    f32 dx = gabi::fsubs_ppc(wordFloat(paneOffset.r3), wordFloat(textOffset.r3));
    f32 dy = gabi::fsubs_ppc(wordFloat(paneOffset.r4), wordFloat(textOffset.r4));
    f32 left = gabi::fadds_ppc(wordFloat(origin.r3), dx);
    f32 top = gabi::fsubs_ppc(wordFloat(origin.r4), dy);
    f32 right = gabi::fadds_ppc(left, extent->width);
    f32 bottom = gabi::fsubs_ppc(top, extent->height);
    bounds->left = left;
    bounds->top = top;
    bounds->right = right;
    bounds->bottom = bottom;
    if (!output) {
        output = static_cast<TextBounds*>(textPaneAllocate(sizeof(TextBounds)));
        left = bounds->left;
    }
    if (output) {
        copyFloatBits(output->left, bounds->left);
        copyFloatBits(output->top, bounds->top);
        copyFloatBits(output->right, bounds->right);
        copyFloatBits(output->bottom, bounds->bottom);
    }
    writer->destroy(2);
}

/* 028785C8 */
void TextPane::update(PaneUpdateContext* context, u32 force) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    LayoutPane::updateMatrix(reinterpret_cast<PaneTraversalContext*>(context), force);
    if (!length || !font || !material) return;
    if (u8(textFlags) & TextPane_Dirty) {
        batch->dirty = 0;
        textFlags = u8(textFlags) & ~u32(TextPane_Dirty);
    }
    if (batch->dirty) return;
    gabi::FrameLocal<TextLayoutWriter> writer(0x24);
    TextLayoutWriter::create(writer.get());
    gabi::call_ptr<void>(paneMethods<PaneMethods>(this)->setupWriter, this, writer.get());
    writer->batch = batch;
    writer->clearBatch();
    writer->drawText(text, length);
    writer->finishBatch();
    auto* quads = reinterpret_cast<nw::ut::QuadBatch*>(batch.get());
    if (u8(textFlags) & TextPane_Shadow) {
        gabi::FrameLocal<nw::ut::ShadowParameters> shadow(8);
        shadow->topColor = 0xFFFFFFFF;
        shadow->bottomColor = 0xFFFFFFFF;
        shadow->topColor = u32(shadowColors[0].value);
        shadow->bottomColor = u32(shadowColors[1].value);
        copyFloatBits(shadow->heightScale, shadowScale[1]);
        copyFloatBits(shadow->x, shadowOffset[0]);
        copyFloatBits(shadow->bearingScale, shadowScale[0]);
        f32 italic = gabi::fmuls_ppc(shadowItalicRatio, fontWidth);
        shadow->advance = italic;
        copyFloatBits(shadow->y, shadowOffset[1]);
        context->renderer->build(quads, shadow.get());
    } else {
        context->renderer->build(quads, nullptr);
    }
    writer->destroy(2);
}

/* 02878740 */
void TextPane::drawBorder(nw::ut::QuadRenderer* renderer, const nw::ut::DrawParameters* source) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    /* The border pass draws flat: the gradient's end colors take the start colors, alphas stay. */
    gabi::FrameLocal<nw::ut::DrawParameters> parameters(8);
    parameters->model = source->model;
    parameters->projection = source->projection;
    f32 red = source->interpolateFrom[0];
    f32 alpha = source->interpolateFrom[3];
    parameters->interpolateFrom[0] = red;
    parameters->interpolateFrom[3] = alpha;
    f32 green = source->interpolateFrom[1];
    f32 shadowRed = source->shadowFrom[0];
    parameters->interpolateFrom[1] = green;
    parameters->shadowFrom[0] = shadowRed;
    f32 blue = source->interpolateFrom[2];
    parameters->view = source->view;
    f32 shadowBlue = source->shadowFrom[2];
    parameters->shadowFrom[2] = shadowBlue;
    parameters->batch = source->batch;
    f32 shadowGreen = source->shadowFrom[1];
    parameters->shadowFrom[1] = shadowGreen;
    parameters->interpolateTo[3] = f32(source->interpolateTo[3]);
    parameters->shadowTo[3] = f32(source->shadowTo[3]);
    parameters->interpolateFrom[2] = blue;
    parameters->interpolateTo[2] = blue;
    parameters->flags = u32(source->flags) & ~1u;
    parameters->shadowFrom[3] = f32(source->shadowFrom[3]);
    parameters->interpolateTo[1] = green;
    parameters->shadowTo[1] = shadowGreen;
    parameters->interpolateTo[0] = red;
    parameters->shadowTo[0] = shadowRed;
    parameters->shadowTo[2] = shadowBlue;
    renderer->draw(parameters.get());
}

/* 028787FC */
void TextPane::drawSelf(LayoutDrawContext* context) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!length || !font || !material) return;
    gabi::FrameLocal<Mtx34> world(0x6C);
    textMatrix(world.get());
    gabi::FrameLocal<nw::ut::DrawParameters> parameters(0x10);
    parameters->projection = reinterpret_cast<nw::ut::Matrix4*>(context);
    parameters->batch = reinterpret_cast<nw::ut::QuadBatch*>(batch.get());
    parameters->view = reinterpret_cast<nw::ut::MatrixRows3*>(world.get());
    setColor(parameters->interpolateFrom, 0.0f, 0.0f, 0.0f, 1.0f);
    setColor(parameters->interpolateTo, 1.0f, 1.0f, 1.0f, 1.0f);
    setColor(parameters->shadowFrom, 0.0f, 0.0f, 0.0f, 1.0f);
    setColor(parameters->shadowTo, 1.0f, 1.0f, 1.0f, 1.0f);
    if (!sTextIdentityGuard.ready) {
        sTextIdentityGuard.ready = 1;
        for (u32 row = 0; row < 3; ++row)
            for (u32 column = 0; column < 4; ++column) sTextIdentity.m[row][column] = row == column ? 1.0f : 0.0f;
    }
    parameters->model = reinterpret_cast<nw::ut::MatrixRows3*>(&sTextIdentity);
    parameters->flags = (u8(textFlags) & TextPane_InvisibleBorder) ? 1 : 0;
    /* the material's two colors become the gradient, the pane's alpha scales the end alpha */
    LayoutMaterial* drawMaterial = material;
    u32 top = drawMaterial->colors[0].value;
    u32 alpha = content()->inheritedAlpha;
    parameters->interpolateFrom[0] = f32(top >> 24) / 255.0f;
    parameters->interpolateFrom[1] = f32((top >> 16) & 0xFF) / 255.0f;
    parameters->interpolateFrom[2] = f32((top >> 8) & 0xFF) / 255.0f;
    parameters->interpolateFrom[3] = f32(alpha) / 255.0f;
    u32 bottom = drawMaterial->colors[1].value;
    parameters->interpolateTo[0] = f32(bottom >> 24) / 255.0f;
    parameters->interpolateTo[1] = f32((bottom >> 16) & 0xFF) / 255.0f;
    parameters->interpolateTo[2] = f32((bottom >> 8) & 0xFF) / 255.0f;
    parameters->interpolateTo[3] = gabi::fmuls_ppc(f32(bottom & 0xFF) / 255.0f, f32(alpha) / 255.0f);
    u32 layout = drawMaterial->layoutFlags;
    if (layout & 0x8000) {
        /* the TEV color pair follows the blocks the layout word counts in the packed storage */
        u32 offset = (layout >> 30) * 28 + ((layout >> 28) & 3) * 20 + (((layout >> 26) & 3) + ((layout >> 22) & 1)) * 8;
        offset += ((layout >> 20) & 3) * 4 + ((layout >> 19) & 1) * 12 + ((layout >> 23) & 7) * 4 + ((layout >> 16) & 7) * 20;
        be<u8>* tevColors = drawMaterial->storage;
        tevColors += offset;
        for (u32 channel = 0; channel < 3; ++channel) parameters->shadowFrom[channel] = f32(u8(tevColors[channel])) / 255.0f;
        parameters->shadowFrom[3] = f32(alpha) / 255.0f;
        for (u32 channel = 0; channel < 3; ++channel) parameters->shadowTo[channel] = f32(u8(tevColors[channel + 3])) / 255.0f;
        parameters->shadowTo[3] = gabi::fmuls_ppc(f32(u8(tevColors[6])) / 255.0f, f32(alpha) / 255.0f);
    } else {
        parameters->shadowFrom[0] = 0.0f;
        parameters->shadowFrom[1] = 0.0f;
        parameters->shadowFrom[2] = 0.0f;
        parameters->shadowTo[0] = 1.0f;
        parameters->shadowTo[1] = 1.0f;
        parameters->shadowTo[2] = 1.0f;
        parameters->shadowFrom[3] = f32(alpha) / 255.0f;
        parameters->shadowTo[3] = f32(alpha) / 255.0f;
    }
    textPaneSetupMaterial(drawMaterial);
    nw::ut::QuadRenderer* renderer = renderDeviceRenderer(context);
    if (u8(textFlags) & TextPane_DoubleDrawnBorder) {
        drawBorder(renderer, parameters.get());
        parameters->flags = u32(parameters->flags) | 1;
    }
    renderer->draw(parameters.get());
    context->vertexBindingPending = 1;
}

/* WWHD glue
data sTextPaneMethods 101806B4
data sTextIdentityGuard 101FD304
data sTextIdentity 101FE0C4
ext 0273AD10 void* textPaneAllocate(u32)
ext 0273AF40 void textPaneDelete(void*)
ext 0287F3DC void* textPaneAlignedAllocate(u32,u32)
ext 0287F3FC void textPaneRelease(void*)
ext C000A858 void textPaneFill(void*,u32,u32)
ext C000A848 void textPaneCopy(void*,const void*,u32)
ext 028F41B0 f32 textPaneFloor(f32)
ext 028EFFD0 void textPaneConstructArray(void*,u32,u32,u32)
ext 028815D8 nw::lyt::LayoutMaterial* textPaneCreateMaterial(void*,void*,nw::lyt::LayoutResourceSet*)
ext 0288247C nw::lyt::LayoutMaterial* textPaneCopyMaterial(void*,nw::lyt::LayoutMaterial*)
ext 028836B8 void textPaneSetupMaterial(nw::lyt::LayoutMaterial*)
ext 02872920 u32 layoutShortNameEqual(const be<u8>*,const be<u8>*)
ext 0287295C u32 layoutLongNameEqual(const be<u8>*,const be<u8>*)
ext 02875C80 nw::lyt::LayoutPane* nw::lyt::LayoutPane::createFromResource(nw::lyt::LayoutPane*,void*,void*)
ext 02875FC4 nw::lyt::LayoutPane* nw::lyt::LayoutPane::copyFrom(nw::lyt::LayoutPane*,nw::lyt::LayoutPane*)
ext 0287613C void nw::lyt::LayoutPane::destroy(u32)
ext 028762C8 Pair32 nw::lyt::LayoutPane::originOffset() const
ext 028766CC void nw::lyt::LayoutPane::updateMatrix(nw::lyt::PaneTraversalContext*,u32)
ext 0286E2E4 void nw::lyt::GlyphWriter::setSize(f32,f32)
ext 0286EA54 u32 nw::lyt::GlyphWriter::batchBytes(u32)
ext 0286EA74 u32 nw::lyt::GlyphWriter::vertexBytes(u32)
ext 0286EA7C nw::lyt::GlyphBatch_l* nw::lyt::GlyphWriter::initializeBatch(nw::lyt::GlyphBatch_l*,u32,u32)
ext 0286EAB0 void nw::lyt::GlyphWriter::clearBatch()
ext 0286EAC8 void nw::lyt::GlyphWriter::finishBatch()
ext 0286FE50 nw::lyt::TextLayoutWriter* nw::lyt::TextLayoutWriter::create(nw::lyt::TextLayoutWriter*)
ext 0286FED0 void nw::lyt::TextLayoutWriter::destroy(u32)
ext 02870654 void nw::lyt::TextLayoutWriter::measureBounds(nw::lyt::TextBounds*,const be<u16>*,s32)
ext 028710D0 f32 nw::lyt::TextLayoutWriter::drawText(const be<u16>*,s32)
ext 028F7C7C void nw::ut::QuadRenderer::build(nw::ut::QuadBatch*,const nw::ut::ShadowParameters*)
ext 028F8250 void nw::ut::QuadRenderer::draw(const nw::ut::DrawParameters*)
*/
