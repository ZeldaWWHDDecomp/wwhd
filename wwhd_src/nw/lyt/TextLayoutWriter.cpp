/* Bitmap text-layout state and line metrics reconstructed from WWHD.
 * Reuses the merged glyph writer interface; no external source or font engine code used. */
#include "nw/lyt/TextLayoutWriter.h"
using namespace nw::lyt;
static WideTagProcessor sDefaultWideProcessor;
struct SignedConversionWords { be<u32> high, low; };

/* A float the original loads and stores again with no other memory access in between keeps its
 * exact bits in the recompiled reference (a signalling NaN is not quieted); a float that waits in
 * an FPR across another load or store is quieted like any f32 value. */
static inline void copyFloatBits(be<f32>& destination, const be<f32>& source) {
    reinterpret_cast<be<u32>&>(destination) = (u32) reinterpret_cast<const be<u32>&>(source);
}

/* 0286FE50 */
TextLayoutWriter* TextLayoutWriter::create(TextLayoutWriter* writer) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!writer) writer = static_cast<TextLayoutWriter*>(textLayoutAllocate(sizeof(TextLayoutWriter)));
    if (writer) {
        textCreateGlyphWriter(writer);
        auto* settings = reinterpret_cast<TextMeasureSettings*>(writer->reserved);
        writer->tabColumns = 4;
        settings->characterSpacing = 0.0f;
        settings->widthLimit = 0x1.fffffep+127f;
        writer->tagProcessor = &sDefaultWideProcessor;
        writer->styleFlags = 0;
        settings->lineSpacing = 0.0f;
    }
    return writer;
}

/* 0286FED0 */
void TextLayoutWriter::destroy(u32 flags) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    if (!this) return;
    textDestroyGlyphWriter(this, 0);
    if (flags & 1) textLayoutDelete(this);
}

/* 0286FF24 */
f32 TextLayoutWriter::lineAdvance() {
    auto frame = gabi::AutoFrame(gabi::current_function());
    FontInterface* current = font;
    s32 nativeHeight = 0;
    if (current) {
        auto* methods = reinterpret_cast<FontLineMethods*>((FontVtable*)current->vtable);
        nativeHeight = gabi::call_ptr<s32>(methods->lineFeed, current);
    }
    gabi::FrameLocal<SignedConversionWords> conversion(8);
    conversion->low = u32(nativeHeight) ^ 0x80000000;
    conversion->high = 0x43300000;
    f32 verticalScale = scaleY;
    f32 height = f32(nativeHeight);
    auto* settings = reinterpret_cast<TextMeasureSettings*>(reserved);
    f32 spacing = settings->lineSpacing;
    return gabi::fmadds(height, verticalScale, spacing);
}

/* 02870654 */
void TextLayoutWriter::measureBounds(TextBounds* bounds, const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TextLayoutWriter> copy(8);
    auto* source = reinterpret_cast<const be<u32>*>(this);
    auto* destination = reinterpret_cast<be<u32>*>(copy.get());
    for (u32 index = 0; index < sizeof(TextLayoutWriter) / sizeof(u32); ++index) {
        u32 word = source[index];
        destination[index] = word;
    }
    copy->measureBoundsMutable(bounds, text, length);
    copy->destroy(2);
}

/* 028706A0 */
f32 TextLayoutWriter::measureWidth(const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TextBounds> bounds(8);
    bounds->bottom = 0.0f;
    bounds->right = 0.0f;
    bounds->top = 0.0f;
    bounds->left = 0.0f;
    measureBounds(bounds.get(), text, length);
    f32 right = bounds->right;
    f32 left = bounds->left;
    return gabi::fsubs_ppc(right, left);
}

/* 028706F0 */
f32 TextLayoutWriter::measureFirstLineWidth(const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<const be<u16>>> cursor(0x18);
    *cursor = text;
    gabi::FrameLocal<TextBounds> bounds(8);
    bounds->left = 0.0f;
    bounds->top = 0.0f;
    bounds->right = 0.0f;
    bounds->bottom = 0.0f;
    gabi::FrameLocal<TextLayoutWriter> copy(0x1C);
    auto* source = reinterpret_cast<const be<u32>*>(this);
    auto* destination = reinterpret_cast<be<u32>*>(copy.get());
    for (u32 index = 0; index < sizeof(TextLayoutWriter) / sizeof(u32); ++index) {
        u32 word = source[index];
        destination[index] = word;
    }
    copy->x = 0.0f;
    copy->y = 0.0f;
    copy->measureLine(bounds.get(), cursor.get(), length);
    f32 right = bounds->right;
    f32 left = bounds->left;
    f32 width = gabi::fsubs_ppc(right, left);
    copy->destroy(2);
    return width;
}

/* 028710D0 */
f32 TextLayoutWriter::drawText(const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<TextLayoutWriter> copy(8);
    auto* source = reinterpret_cast<const be<u32>*>(this);
    auto* destination = reinterpret_cast<be<u32>*>(copy.get());
    for (u32 index = 0; index < sizeof(TextLayoutWriter) / sizeof(u32); ++index) {
        u32 word = source[index];
        destination[index] = word;
    }
    f32 width = copy->drawTextMutable(text, length);
    f32 horizontal = copy->x;
    f32 vertical = copy->y;
    x = horizontal;
    y = vertical;
    copy->destroy(2);
    return width;
}


/* 02870794 */
f32 TextLayoutWriter::alignTextOrigin(be<f32>* horizontal, be<f32>* vertical, const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    u32 flags = styleFlags;
    u32 alignment = flags & 0x333;
    f32 width = 0.0f;
    f32 height = 0.0f;
    if (alignment != 0x300 && alignment != 0) {
        gabi::FrameLocal<TextBounds> bounds(8);
        bounds->left = 0.0f;
        bounds->top = 0.0f;
        bounds->right = 0.0f;
        bounds->bottom = 0.0f;
        measureBounds(bounds.get(), text, length);
        f32 bottom = bounds->bottom;
        f32 left = bounds->left;
        f32 right = bounds->right;
        f32 top = bounds->top;
        width = gabi::fadds_ppc(left, right);
        height = gabi::fadds_ppc(top, bottom);
        flags = styleFlags;
    }
    u32 horizontalMode = flags & 0x30;
    if (horizontalMode == 0x10) {
        f32 adjustment = textLayoutFloor(gabi::fmuls_ppc(width, 0.5f));
        f32 current = *horizontal;
        *horizontal = gabi::fsubs_ppc(current, adjustment);
        flags = styleFlags;
    } else if (horizontalMode == 0x20) {
        f32 current = *horizontal;
        *horizontal = gabi::fsubs_ppc(current, width);
        flags = styleFlags;
    }
    u32 verticalMode = flags & 0x300;
    if (verticalMode == 0x100) {
        f32 adjustment = textLayoutFloor(gabi::fmuls_ppc(height, 0.5f));
        f32 current = *vertical;
        *vertical = gabi::fsubs_ppc(current, adjustment);
        flags = styleFlags;
    } else if (verticalMode == 0x200) {
        f32 current = *vertical;
        *vertical = gabi::fsubs_ppc(current, height);
        flags = styleFlags;
    }
    u32 lineMode = flags & 3;
    if (lineMode == 1) {
        f32 firstWidth = measureFirstLineWidth(text, length);
        f32 fullHalf = textLayoutFloor(gabi::fmuls_ppc(width, 0.5f));
        f32 lineHalf = textLayoutFloor(gabi::fmuls_ppc(firstWidth, 0.5f));
        f32 offset = gabi::fsubs_ppc(fullHalf, lineHalf);
        f32 current = *horizontal;
        flags = styleFlags;
        x = gabi::fadds_ppc(current, offset);
    } else if (lineMode == 2) {
        f32 firstWidth = measureFirstLineWidth(text, length);
        f32 offset = gabi::fsubs_ppc(width, firstWidth);
        f32 current = *horizontal;
        flags = styleFlags;
        x = gabi::fadds_ppc(current, offset);
    } else {
        flags = styleFlags;
        copyFloatBits(x, *horizontal);
    }
    if ((flags & 0x300) != 0x300) {
        f32 ascent = textLayoutAscent(this);
        f32 current = *vertical;
        y = gabi::fadds_ppc(current, ascent);
    } else {
        copyFloatBits(y, *vertical);
    }
    return width;
}

/* WWHD glue
ext 028F41B0 f32 textLayoutFloor(f32)
ext 0286E494 f32 textLayoutAscent(nw::lyt::GlyphWriter*)
ext 0273AD10 void* textLayoutAllocate(u32)
ext 0273AF40 void textLayoutDelete(void*)
ext 0286E20C nw::lyt::GlyphWriter* textCreateGlyphWriter(nw::lyt::GlyphWriter*)
ext 0286E2D0 void textDestroyGlyphWriter(nw::lyt::GlyphWriter*, u32)
data sDefaultWideProcessor 104B6844 nocheck
*/
