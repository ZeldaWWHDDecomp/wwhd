/* Text box pane of nw::lyt: a 0x104-byte pane that owns a UTF-16 buffer, its glyph quad batch and
 * one material. Layout from the constructor (sizeof through the NULL-this allocation), the accessors
 * and the method table at 101806B4; resource offsets are the BFLYT txt1 block (format facts only).
 * Built on the merged pane, draw context, material, text writer and quad renderer types. */
#pragma once
#include "nw/lyt/PaneState.h"
#include "nw/lyt/LayoutDrawContext.h"
#include "nw/lyt/LayoutMaterial.h"
#include "nw/lyt/TextLayoutWriter.h"
#include "nw/lyt/ResourceProvider.h"
#include "nw/lyt/ResourceHelpers.h"
#include "nw/ut/QuadRenderer.h"
namespace nw { namespace lyt {

/* The pane method-table slots this unit calls (8-byte GHS entries; named after their targets). */
struct PaneMethods {
    u8 beforeMaterialAt[0x4C]; be<u32> materialAt;
    u8 beforeMaterialCount[4]; be<u32> materialCount;
    u8 beforeFindMaterial[0xC]; be<u32> findMaterial;
    u8 beforeDraw[0x24]; be<u32> draw;
    u8 beforeDrawSelf[4]; be<u32> drawSelf;
    u8 beforeFindPaneByName[0xC]; be<u32> findPaneByName;
    u8 beforeFindMaterialByName[4]; be<u32> findMaterialByName;
    u8 beforeReleaseText[0xC]; be<u32> releaseText;
    u8 beforeSetupWriter[0x14]; be<u32> setupWriter;
};
WWHD_OFFSET(PaneMethods, materialAt, 0x4C);
WWHD_OFFSET(PaneMethods, materialCount, 0x54);
WWHD_OFFSET(PaneMethods, findMaterial, 0x64);
WWHD_OFFSET(PaneMethods, draw, 0x8C);
WWHD_OFFSET(PaneMethods, drawSelf, 0x94);
WWHD_OFFSET(PaneMethods, findPaneByName, 0xA4);
WWHD_OFFSET(PaneMethods, findMaterialByName, 0xAC);
WWHD_OFFSET(PaneMethods, releaseText, 0xBC);
WWHD_OFFSET(PaneMethods, setupWriter, 0xD4);

/* Material slots used by the panes that own materials. */
struct MaterialMethods {
    u8 beforeDestroy[0xC]; be<u32> destroy;
    u8 beforeSetup[0x14]; be<u32> setup;
};
WWHD_OFFSET(MaterialMethods, destroy, 0xC);
WWHD_OFFSET(MaterialMethods, setup, 0x24);
inline MaterialMethods* materialMethods(const LayoutMaterial* material) {
    return gabi::at<MaterialMethods>(gabi::ea(material->methods.get()));
}
enum MaterialOwnership : u32 { Material_Shared = 1 };

/* The font and material tables and the font provider of the resources a pane is built from. */
struct ResourceNameTable {
    u8 header[0xC];
    be<u32> offsets[1];
    /* fnl1: name offsets count from the offset table */
    const be<u8>* nameAt(u32 index) { return layoutAtOffset<const be<u8>>(offsets, offsets[index]); }
    /* mat1: material offsets count from the block header */
    void* blockEntry(u32 index) { return layoutAtOffset<void>(this, offsets[index]); }
};
struct FontProviderMethods { u8 beforeFindFont[0x24]; be<u32> findFont; };
struct LayoutResourceSet {
    gptr<void> layout;
    gptr<ResourceNameTable> fonts;
    gptr<ResourceNameTable> materials;
    gptr<ResourceProvider> provider;
    gptr<void> userData;
};
WWHD_OFFSET(ResourceNameTable, offsets, 0xC);
WWHD_OFFSET(FontProviderMethods, findFont, 0x24);
WWHD_SIZE(LayoutResourceSet, 0x14);

/* Optional text search callback of a layout build: fills a result with the text for a text ID. */
struct TextSearchResult {
    gptr<const be<u16>> text;
    be<u16> reserved, length;
    be<u32> bufferLength;
    be<s32> maximumLength;
};
struct TextSearcherMethods { u8 beforeSearch[0x14]; be<u32> search; };
struct TextSearcher { gptr<TextSearcherMethods> methods; };
WWHD_SIZE(TextSearchResult, 0x10);
WWHD_OFFSET(TextSearcherMethods, search, 0x14);

struct PaneBuildContext {
    u8 header[0x14];
    gptr<TextSearcher> textSearcher;
    gptr<void> searchUserData;
    gptr<LayoutResourceSet> resources;
    gptr<LayoutResourceSet> overrideResources;
    be<u16> flags;
};
enum PaneBuildFlag : u32 { PaneBuild_KeepOwnResource = 1 };
WWHD_OFFSET(PaneBuildContext, textSearcher, 0x14);
WWHD_OFFSET(PaneBuildContext, flags, 0x24);

/* BFLYT txt1 block after the 0x54-byte pane header. */
struct TextBoxResource {
    u8 pane[0x54];
    be<u16> bufferBytes, textBytes, materialIndex, fontIndex;
    be<u8> position, alignment, flags, reserved;
    be<f32> italicRatio;
    be<u32> textOffset;
    ColorWord colors[2];
    be<f32> fontWidth, fontHeight, characterSpace, lineSpace;
    be<u32> textIdOffset;
    be<f32> shadowOffset[2], shadowScale[2];
    ColorWord shadowColors[2];
    be<f32> shadowItalicRatio;
};
enum TextBoxResourceFlag : u32 {
    TextBoxResource_Shadow = 1, TextBoxResource_RestrictLength = 2,
    TextBoxResource_InvisibleBorder = 4, TextBoxResource_DoubleDrawnBorder = 8,
};
WWHD_OFFSET(TextBoxResource, italicRatio, 0x60);
WWHD_OFFSET(TextBoxResource, textIdOffset, 0x80);
WWHD_SIZE(TextBoxResource, 0xA0);

enum TextPaneFlag : u32 {
    TextPane_WidthLimit = 2, TextPane_DoubleDrawnBorder = 4, TextPane_InvisibleBorder = 8,
    TextPane_Shadow = 0x10, TextPane_Dirty = 0x20, TextPane_AlignmentShift = 6,
};
/* Two-bit horizontal (bits 0-1) and vertical (bits 2-3) position codes. */
enum TextPosition : u32 { TextPosition_Center = 0, TextPosition_Far = 2 };

/* The state a pane update receives (same object as PaneMatrix.cpp's PaneTraversalContext). */
struct PaneUpdateContext {
    gptr<nw::ut::QuadRenderer> renderer;
    gptr<Mtx34> rootMatrix;
    be<f32> scaleX, scaleY, alpha;
    be<u8> scaleEnabled, showInvisible, showTransparent, alphaEnabled;
    static PaneUpdateContext* create(PaneUpdateContext* context, LayoutDrawContext* drawContext);
};
WWHD_SIZE(PaneUpdateContext, 0x18);
/* The quad renderer lives 8 bytes into the render device. */
struct RenderDeviceView { u8 header[8]; nw::ut::QuadRenderer renderer; };
inline nw::ut::QuadRenderer* deviceRenderer(LayoutDrawContext* context) {
    return &reinterpret_cast<RenderDeviceView*>(context->device.get())->renderer;
}
/* Initial draw-flag byte of LayoutDrawContext (merged as initializedByte). */
enum DrawContextFlag : u32 { Draw_ScaleEnabled = 0x80, Draw_ShowInvisible = 0x40, Draw_ShowTransparent = 0x20 };
struct PaneSize { be<f32> width, height; };
WWHD_SIZE(PaneSize, 8);

/* Out-of-line copies of LayoutPane's inline virtuals that GHS emitted in this translation unit. */
struct LayoutPaneInlines : LayoutPane {
    void draw(void* context);
    void drawSelf(LayoutDrawContext* context);
    void loadMatrix(LayoutDrawContext* context);
    LayoutPane* findPaneByName(const be<u8>* name);
    LayoutMaterial* findMaterialByName(const be<u8>* name);
    u32 materialCount();
};
RuntimeTypeNode* paneRuntimeType();

struct TextPane : LayoutPane {
    gptr<be<u16>> text;
    gptr<const be<u16>> textId;
    ColorWord colors[2];
    gptr<FontInterface> font;
    be<f32> fontWidth, fontHeight, lineSpace, characterSpace;
    gptr<WideTagProcessor> tagProcessor;
    be<u16> bufferLength, length;
    be<u8> position;
    u8 positionPadding[3];
    be<f32> italicRatio;
    be<f32> shadowOffset[2], shadowScale[2];
    ColorWord shadowColors[2];
    be<f32> shadowItalicRatio;
    be<u8> textFlags;
    u8 flagsPadding[3];
    gptr<LayoutMaterial> material;
    gptr<GlyphBatch_l> batch;
    gptr<void> vertices;

    static TextPane* createFromResource(TextPane* pane, TextBoxResource* resource, TextBoxResource* overrideResource, PaneBuildContext* context);
    static TextPane* copyFrom(TextPane* pane, TextPane* source);
    void destroy(u32 flags);
    static RuntimeTypeNode* runtimeType();
    void initializeText();
    void reserveText(u32 length);
    void releaseText();
    void setFont(FontInterface* replacement);
    FontInterface* getFont();
    u32 textBufferLength();
    u32 copyText(const be<u16>* source, u32 offset, u32 count);
    u32 setText(const be<u16>* source, u32 offset);
    u32 setTextCount(const be<u16>* source, u32 offset, u32 count);
    void setColorElement(u32 index, u32 value);
    u32 materialCount();
    LayoutMaterial* materialAt(u32 index);
    Pair32 alignmentOffset(const PaneSize* size, u32 round);
    void applyFont(TextLayoutWriter* writer);
    void setupWriter(TextLayoutWriter* writer);
    void setupAlignment(TextLayoutWriter* writer);
    void textBounds(TextBounds* output);
    void textMatrix(Mtx34* destination);
    void update(PaneUpdateContext* context, u32 force);
    void drawBorder(nw::ut::QuadRenderer* renderer, const nw::ut::DrawParameters* source);
    void drawSelf(LayoutDrawContext* context);
    void loadMatrix(LayoutDrawContext* context);
    PaneSize* size() { return reinterpret_cast<PaneSize*>(&content()->width); }
};
ColorWord* createTextColor(ColorWord* color);
WWHD_OFFSET(TextPane, text, 0xA4);
WWHD_OFFSET(TextPane, font, 0xB4);
WWHD_OFFSET(TextPane, tagProcessor, 0xC8);
WWHD_OFFSET(TextPane, bufferLength, 0xCC);
WWHD_OFFSET(TextPane, italicRatio, 0xD4);
WWHD_OFFSET(TextPane, shadowColors, 0xE8);
WWHD_OFFSET(TextPane, textFlags, 0xF4);
WWHD_OFFSET(TextPane, material, 0xF8);
WWHD_SIZE(TextPane, 0x104);
WWHD_OFFSET(PaneContent, width, 0x30);
} }

void* textPaneAllocate(u32 size);
void textPaneDelete(void* storage);
void* textPaneAlignedAllocate(u32 size, u32 alignment);
void textPaneRelease(void* storage);
void textPaneFill(void* storage, u32 value, u32 size);
void textPaneCopy(void* destination, const void* source, u32 size);
f32 textPaneFloor(f32 value);
void textPaneConstructArray(void* array, u32 count, u32 stride, u32 constructor);
nw::lyt::LayoutMaterial* textPaneCreateMaterial(void* storage, void* resource, nw::lyt::LayoutResourceSet* resources);
nw::lyt::LayoutMaterial* textPaneCopyMaterial(void* storage, nw::lyt::LayoutMaterial* source);
void textPaneSetupMaterial(nw::lyt::LayoutMaterial* material);
