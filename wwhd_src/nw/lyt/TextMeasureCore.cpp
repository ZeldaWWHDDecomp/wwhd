/* Text line scanning, tag measurement and cursor advancement from WWHD.
 * The reader's GHS member-pointer union is preserved, including its virtual form. */
#include "nw/lyt/TextLayoutWriter.h"
#include "nw/lyt/ResourceHelpers.h"
using namespace nw::lyt;
struct TextCodeReader {
    gptr<const be<u16>> cursor;
    be<s16> receiverAdjustment, virtualIndex;
    union { be<u32> decoder; struct { u8 reserved[2]; be<s16> tableOffset; }; };
};
struct TextScanContext {
    gptr<TextLayoutWriter> writer;
    gptr<const be<u16>> cursor;
    be<u32> end;
    be<f32> originX, originY, scaleX, scaleY;
    be<u32> flags;
    be<u16> previous;
    u8 padding[2];
};
struct TextTagMethods { u8 prefix[0x24]; be<u32> measure; };
struct TextMetricMethods { u8 prefix[0x7C]; be<u32> width; u8 middle[0x1C]; be<u32> kerning; };
struct TextConversion { be<u32> high, low; };
void textInitializeReader(FontInterface*, TextCodeReader*, u32);
/* A UTF-16 text position kept as a guest address. */
static inline const be<u16>* textCursorAt(u32 address) { return gabi::at<const be<u16>>(address); }
/* The tag processor's method table viewed as T. */
template <class T> static inline T* tagMethodsOf(WideTagProcessor* processor) { return gabi::at<T>(processor->vtable); }

static u32 readTextCode(TextCodeReader* reader) {
    u32 receiver = gabi::ea(reader) + s32(reader->receiverAdjustment);
    s32 index = reader->virtualIndex;
    u32 method;
    if (index < 0) method = reader->decoder;
    else {
        s32 offset = reader->tableOffset;
        u32 table = *gabi::at<be<u32>>(receiver + offset);
        method = *gabi::at<be<u32>>(table + u32(index) * 8 + 4);
    }
    return gabi::call_ptr<u32>(method, gabi::at<void>(receiver));
}

static void mergeLineBounds(TextBounds* output, const TextBounds* input) {
    f32 inputLeft = input->left;
    f32 left = output->left;
    f32 leftDifference = gabi::fsubs_ppc(left, inputLeft);
    f32 selectedLeft = leftDifference >= 0.0f ? inputLeft : left;
    f32 inputTop = input->top;
    f32 top = output->top;
    f32 topDifference = gabi::fsubs_ppc(top, inputTop);
    f32 selectedTop = topDifference >= 0.0f ? inputTop : top;
    f32 right = output->right;
    f32 inputRight = input->right;
    f32 rightDifference = gabi::fsubs_ppc(right, inputRight);
    f32 selectedRight = rightDifference >= 0.0f ? right : inputRight;
    f32 inputBottom = input->bottom;
    f32 bottom = output->bottom;
    f32 bottomDifference = gabi::fsubs_ppc(bottom, inputBottom);
    output->top = selectedTop;
    output->left = selectedLeft;
    f32 selectedBottom = bottomDifference >= 0.0f ? bottom : inputBottom;
    output->right = selectedRight;
    output->bottom = selectedBottom;
}

/* 0286FFCC */
u32 TextLayoutWriter::measureLine(TextBounds* bounds, gptr<const be<u16>>* cursor, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* settings = reinterpret_cast<TextMeasureSettings*>(reserved);
    bool limited = f32(settings->widthLimit) < 0x1.fffffep+127f;
    gabi::FrameLocal<TextScanContext> state(0x30);
    state->writer = this;
    f32 verticalScale = scaleY;
    const be<u16>* first = *cursor;
    u32 byteLength = u32(length) * 2;
    u32 end = gabi::ea(first) + byteLength;
    state->previous = 0;
    state->scaleY = verticalScale;
    state->flags = 0;
    state->originY = 0.0f;
    state->end = end;
    state->cursor = first;
    state->scaleX = scaleX;
    state->originX = 0.0f;
    gabi::FrameLocal<TextCodeReader> reader(0xC4);
    textInitializeReader(font, reader.get(), 0);
    f32 advanceHeight = lineAdvance();
    f32 bearingHeight = settings->lineSpacing;
    f32 extent = gabi::fsubs_ppc(advanceHeight, bearingHeight);
    s32 readerIndex = reader->virtualIndex;
    s32 receiverAdjustment = reader->receiverAdjustment;
    reader->cursor = first;
    bounds->right = 0.0f;
    bounds->left = 0.0f;
    bounds->top = extent >= 0.0f ? 0.0f : extent;
    bounds->bottom = extent >= 0.0f ? extent : 0.0f;
    (void)readerIndex;
    (void)receiverAdjustment;
    bool spacing = false;
    u32 previousCursor = 0;
    f32 lineX = 0.0f;
    u32 wrapped = 0;
    u32 character = readTextCode(reader.get());
    u32 current = gabi::ea((const be<u16>*)reader->cursor);
    while (current <= end) {
        if (character < 32) {
            gabi::FrameLocal<TextBounds> tagBounds(0x10);
            tagBounds->left = lineX;
            state->cursor = textCursorAt(current);
            state->flags = u32(!spacing);
            tagBounds->bottom = 0.0f;
            tagBounds->right = 0.0f;
            x = lineX;
            tagBounds->top = 0.0f;
            if (limited && character != 10 && previousCursor != 0) {
                gabi::FrameLocal<TextScanContext> previewState(0x54);
                auto* source = reinterpret_cast<const be<u32>*>(state.get());
                auto* destination = reinterpret_cast<be<u32>*>(previewState.get());
                for (u32 index = 0; index < 9; ++index) { u32 word = source[index]; destination[index] = word; }
                gabi::FrameLocal<TextLayoutWriter> previewWriter(0x78);
                source = reinterpret_cast<const be<u32>*>(this);
                destination = reinterpret_cast<be<u32>*>(previewWriter.get());
                for (u32 index = 0; index < 19; ++index) { u32 word = source[index]; destination[index] = word; }
                gabi::FrameLocal<TextBounds> previewBounds(0x20);
                previewBounds->left = 0.0f;
                previewState->writer = previewWriter.get();
                previewBounds->right = 0.0f;
                previewBounds->bottom = 0.0f;
                previewBounds->top = 0.0f;
                WideTagProcessor* processor = tagProcessor;
                auto* methods = tagMethodsOf<TextTagMethods>(processor);
                gabi::call_ptr<u32>(methods->measure, processor, previewBounds.get(), character, previewState.get());
                f32 previewLeft = previewBounds->left;
                f32 previewRight = previewBounds->right;
                f32 previewWidth = gabi::fsubs_ppc(previewRight, previewLeft);
                bool breakLine = false;
                if (previewWidth > 0.0f) {
                    f32 previewX = previewWriter->x;
                    f32 origin = state->originX;
                    f32 width = gabi::fsubs_ppc(previewX, origin);
                    f32 limit = settings->widthLimit;
                    breakLine = width > limit;
                }
                if (breakLine) {
                    wrapped = 1;
                    state->previous = 0;
                    reader->cursor = textCursorAt(previousCursor);
                    character = 10;
                }
                previewWriter->destroy(2);
                if (breakLine) { current = gabi::ea((const be<u16>*)reader->cursor); continue; }
            }
            WideTagProcessor* processor = tagProcessor;
            auto* methods = tagMethodsOf<TextTagMethods>(processor);
            u32 action = gabi::call_ptr<u32>(methods->measure, processor, tagBounds.get(), character, state.get());
            // The callback may replace the cursor; restore it before reading another code point.
            u32 callbackCursor = gabi::ea((const be<u16>*)state->cursor);
            mergeLineBounds(bounds, tagBounds.get());
            reader->cursor = textCursorAt(callbackCursor);
            lineX = x;
            current = callbackCursor;
            if (action == 3) break;
            if (action == 4) {
                const be<u16>* original = *cursor;
                *cursor = layoutAtOffset<const be<u16>>(original, byteLength);
                return 0;
            }
            if (action == 1) spacing = false;
            else if (action == 2) spacing = true;
        } else {
            f32 nextX = lineX;
            u32 fixed = fixedAdvance;
            if (spacing) nextX = gabi::fadds_ppc(nextX, settings->characterSpacing);
            if (fixed) {
                FontInterface* currentFont = font;
                auto* methods = reinterpret_cast<TextMetricMethods*>((FontVtable*)currentFont->vtable);
                nextX = gabi::fadds_ppc(nextX, advance);
                u32 previous = state->previous;
                s32 kerning = gabi::call_ptr<s32>(methods->kerning, currentFont, previous, character);
                gabi::FrameLocal<TextConversion> conversion(8);
                conversion->high = 0x43300000;
                conversion->low = u32(kerning) ^ 0x80000000;
                f32 horizontalScale = scaleX;
                nextX = gabi::fmadds(f32(kerning), horizontalScale, nextX);
            } else {
                FontInterface* currentFont = font;
                auto* methods = reinterpret_cast<TextMetricMethods*>((FontVtable*)currentFont->vtable);
                s32 width = gabi::call_ptr<s32>(methods->width, currentFont, character);
                gabi::FrameLocal<TextConversion> conversion(8);
                conversion->low = u32(width) ^ 0x80000000;
                currentFont = font;
                conversion->high = 0x43300000;
                methods = reinterpret_cast<TextMetricMethods*>((FontVtable*)currentFont->vtable);
                f32 horizontalScale = scaleX;
                nextX = gabi::fmadds(f32(width), horizontalScale, nextX);
                u32 previous = state->previous;
                s32 kerning = gabi::call_ptr<s32>(methods->kerning, currentFont, previous, character);
                conversion->high = 0x43300000;
                conversion->low = u32(kerning) ^ 0x80000000;
                horizontalScale = scaleX;
                nextX = gabi::fmadds(f32(kerning), horizontalScale, nextX);
            }
            if (limited && previousCursor != 0 && nextX > f32(settings->widthLimit)) {
                current = previousCursor;
                wrapped = 1;
                state->previous = 0;
                character = 10;
                reader->cursor = textCursorAt(current);
                continue;
            }
            f32 left = bounds->left;
            f32 leftDifference = gabi::fsubs_ppc(left, nextX);
            f32 selectedLeft = leftDifference >= 0.0f ? nextX : left;
            f32 right = bounds->right;
            f32 rightDifference = gabi::fsubs_ppc(right, nextX);
            state->previous = character;
            f32 selectedRight = rightDifference >= 0.0f ? right : nextX;
            bounds->left = selectedLeft;
            spacing = true;
            lineX = nextX;
            bounds->right = selectedRight;
        }
        if (limited) previousCursor = gabi::ea((const be<u16>*)reader->cursor);
        character = readTextCode(reader.get());
        current = gabi::ea((const be<u16>*)reader->cursor);
    }
    *cursor = textCursorAt(current);
    return wrapped;
}

/* 02870554 */
void TextLayoutWriter::measureBoundsMutable(TextBounds* bounds, const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    gabi::FrameLocal<gptr<const be<u16>>> cursor(0x18);
    *cursor = text;
    bounds->bottom = 0.0f;
    bounds->top = 0.0f;
    bounds->left = 0.0f;
    u32 end = gabi::ea(text) + u32(length) * 2;
    bounds->right = 0.0f;
    x = 0.0f;
    y = 0.0f;
    do {
        gabi::FrameLocal<TextBounds> line(8);
        line->left = 0.0f;
        line->top = 0.0f;
        line->right = 0.0f;
        line->bottom = 0.0f;
        measureLine(line.get(), cursor.get(), length);
        mergeLineBounds(bounds, line.get());
        length = s32(end - gabi::ea((const be<u16>*)*cursor)) >> 1;
    } while (length > 0);
}


struct TextDrawTagMethods {
    u8 prefix[0x1C]; be<u32> process; u8 gap20[4]; be<u32> measure;
    be<u32> gap28, begin, gap30, end;
};
struct TextDrawFontMethods {
    u8 prefix[0x8C]; be<u32> glyph; u8 gap90[0xC]; be<u32> kerning;
    u8 gapA0[0xC]; be<u32> baseline;
};
struct TextOriginCoordinate { be<f32> value; };
void textCreateGlyphRecord(GlyphRecord*);
f32 textPlaceGlyphRecord(GlyphWriter*, GlyphRecord*);

/* 028709E0 */
f32 TextLayoutWriter::drawTextMutable(const be<u16>* text, s32 length) {
    auto frame = gabi::AutoFrame(gabi::current_function());
    auto* settings = reinterpret_cast<TextMeasureSettings*>(reserved);
    f32 limit = settings->widthLimit;
    bool limited = limit < 0x1.fffffep+127f;
    f32 originalX = x;
    f32 originalY = y;
    gabi::FrameLocal<TextOriginCoordinate> horizontal(0x44);
    gabi::FrameLocal<TextOriginCoordinate> vertical(0x60);
    vertical->value = originalY;
    horizontal->value = originalX;
    f32 maximumWidth = alignTextOrigin(&horizontal->value, &vertical->value, text, length);
    f32 horizontalScale = scaleX;
    f32 alignedY = y;
    f32 originX = horizontal->value;
    f32 verticalScale = scaleY;
    f32 verticalAdjustment = gabi::fsubs_ppc(originalY, alignedY);
    gabi::FrameLocal<TextScanContext> state(0x20);
    state->end = gabi::ea(text) + u32(length) * 2;
    state->scaleX = horizontalScale;
    state->scaleY = verticalScale;
    state->writer = this;
    f32 originY = vertical->value;
    state->cursor = text;
    state->originY = originY;
    state->previous = 0;
    state->flags = 0;
    state->originX = originX;
    gabi::FrameLocal<TextCodeReader> reader(0xD4);
    textInitializeReader(font, reader.get(), 0);
    WideTagProcessor* processor = tagProcessor;
    reader->cursor = text;
    auto* tagMethods = tagMethodsOf<TextDrawTagMethods>(processor);
    gabi::call_ptr<void>(tagMethods->begin, processor, state.get());
    u32 previousCursor = gabi::ea(text);
    u32 lineStart = gabi::ea(text);
    bool spacing = false;
    f32 lineWidth = 0.0f;
    u32 character = readTextCode(reader.get());
    u32 current = gabi::ea((const be<u16>*)reader->cursor);
    f32 finalX = 0.0f;
    bool cachedFinalX = false;
    while ((s32(current - gabi::ea(text)) >> 1) <= length) {
        if (character < 32) {
            state->cursor = textCursorAt(current);
            state->flags = u32(!spacing);
            if (limited && character != 10 && previousCursor != lineStart) {
                gabi::FrameLocal<TextScanContext> previewState(0x64);
                auto* source = reinterpret_cast<const be<u32>*>(state.get());
                auto* destination = reinterpret_cast<be<u32>*>(previewState.get());
                for (u32 index = 0; index < 9; ++index) { u32 word = source[index]; destination[index] = word; }
                gabi::FrameLocal<TextLayoutWriter> previewWriter(0x88);
                source = reinterpret_cast<const be<u32>*>(this);
                destination = reinterpret_cast<be<u32>*>(previewWriter.get());
                for (u32 index = 0; index < 19; ++index) { u32 word = source[index]; destination[index] = word; }
                gabi::FrameLocal<TextBounds> previewBounds(0x10);
                previewBounds->right = 0.0f;
                previewBounds->bottom = 0.0f;
                previewBounds->left = 0.0f;
                processor = tagProcessor;
                previewState->writer = previewWriter.get();
                previewBounds->top = 0.0f;
                tagMethods = tagMethodsOf<TextDrawTagMethods>(processor);
                gabi::call_ptr<u32>(tagMethods->measure, processor, previewBounds.get(), character, previewState.get());
                f32 left = previewBounds->left;
                f32 right = previewBounds->right;
                f32 width = gabi::fsubs_ppc(right, left);
                bool breakLine = false;
                if (width > 0.0f) {
                    f32 previewX = previewWriter->x;
                    f32 origin = state->originX;
                    f32 extent = gabi::fsubs_ppc(previewX, origin);
                    breakLine = extent > f32(settings->widthLimit);
                }
                if (breakLine) {
                    state->previous = 0;
                    reader->cursor = textCursorAt(previousCursor);
                    character = 10;
                }
                previewWriter->destroy(2);
                if (breakLine) { current = gabi::ea((const be<u16>*)reader->cursor); continue; }
            }
            processor = tagProcessor;
            tagMethods = tagMethodsOf<TextDrawTagMethods>(processor);
            u32 action = gabi::call_ptr<u32>(tagMethods->process, processor, character, state.get());
            f32 base = horizontal->value;
            f32 currentX = x;
            lineWidth = gabi::fsubs_ppc(currentX, base);
            if (action == 4) { finalX = currentX; cachedFinalX = true; break; }
            if (action == 3) {
                u32 mode = u32(styleFlags) & 3;
                if (mode == 1) {
                    const be<u16>* next = state->cursor;
                    s32 consumed = s32(gabi::ea(next) - gabi::ea(text)) >> 1;
                    f32 firstWidth = measureFirstLineWidth(next, s32(u32(length) - u32(consumed)));
                    f32 fullHalf = textLayoutFloor(gabi::fmuls_ppc(maximumWidth, 0.5f));
                    f32 lineHalf = textLayoutFloor(gabi::fmuls_ppc(firstWidth, 0.5f));
                    f32 adjustment = gabi::fsubs_ppc(fullHalf, lineHalf);
                    f32 origin = state->originX;
                    x = gabi::fadds_ppc(origin, adjustment);
                } else if (mode == 2) {
                    const be<u16>* next = state->cursor;
                    s32 consumed = s32(gabi::ea(next) - gabi::ea(text)) >> 1;
                    f32 firstWidth = measureFirstLineWidth(next, s32(u32(length) - u32(consumed)));
                    f32 adjustment = gabi::fsubs_ppc(maximumWidth, firstWidth);
                    f32 origin = state->originX;
                    x = gabi::fadds_ppc(origin, adjustment);
                } else {
                    f32 origin = state->originX;
                    f32 width = gabi::fsubs_ppc(currentX, origin);
                    if (maximumWidth < width) maximumWidth = width;
                    x = origin;
                }
                if (limited) lineStart = gabi::ea((const be<u16>*)reader->cursor);
                spacing = false;
            } else if (action == 1) spacing = false;
            else if (action == 2) spacing = true;
            reader->cursor = state->cursor;
        } else {
            FontInterface* currentFont = font;
            auto* fontMethods = reinterpret_cast<TextDrawFontMethods*>((FontVtable*)currentFont->vtable);
            f32 oldY = y;
            u32 previous = state->previous;
            s32 kerning = gabi::call_ptr<s32>(fontMethods->kerning, currentFont, previous, character);
            gabi::FrameLocal<TextConversion> conversion(8);
            conversion->high = 0x43300000;
            conversion->low = u32(kerning) ^ 0x80000000;
            f32 scale = scaleX;
            f32 kernAdvance = gabi::fmuls_ppc(f32(kerning), scale);
            gabi::FrameLocal<GlyphRecord> glyph(0x48);
            textCreateGlyphRecord(glyph.get());
            currentFont = font;
            fontMethods = reinterpret_cast<TextDrawFontMethods*>((FontVtable*)currentFont->vtable);
            gabi::call_ptr<void>(fontMethods->glyph, currentFont, glyph.get(), character);
            u32 fixed = fixedAdvance;
            f32 nextWidth = lineWidth;
            if (spacing) nextWidth = gabi::fadds_ppc(nextWidth, settings->characterSpacing);
            if (fixed) nextWidth = gabi::fadds_ppc(nextWidth, advance);
            else {
                u32 glyphAdvance = glyph->metrics.advance;
                conversion->low = glyphAdvance;
                conversion->high = 0x43300000;
                f32 currentScale = scaleX;
                nextWidth = gabi::fmadds(f32(glyphAdvance), currentScale, nextWidth);
            }
            nextWidth = gabi::fadds_ppc(nextWidth, kernAdvance);
            if (limited && previousCursor != lineStart && nextWidth > f32(settings->widthLimit)) {
                current = previousCursor;
                state->previous = 0;
                character = 10;
                reader->cursor = textCursorAt(current);
                continue;
            }
            f32 currentX = x;
            lineWidth = nextWidth;
            currentFont = font;
            if (spacing) currentX = gabi::fadds_ppc(currentX, settings->characterSpacing);
            x = gabi::fadds_ppc(currentX, kernAdvance);
            fontMethods = reinterpret_cast<TextDrawFontMethods*>((FontVtable*)currentFont->vtable);
            spacing = true;
            s32 baseline = gabi::call_ptr<s32>(fontMethods->baseline, currentFont);
            u32 negative = 0 - u32(baseline);
            conversion->high = 0x43300000;
            conversion->low = negative ^ 0x80000000;
            f32 currentY = y;
            f32 currentScaleY = scaleY;
            y = gabi::fmadds(f32(s32(negative)), currentScaleY, currentY);
            textPlaceGlyphRecord(this, glyph.get());
            state->previous = character;
            y = oldY;
        }
        if (limited) previousCursor = gabi::ea((const be<u16>*)reader->cursor);
        character = readTextCode(reader.get());
        current = gabi::ea((const be<u16>*)reader->cursor);
    }
    if (!cachedFinalX) finalX = x;
    f32 base = state->originX;
    f32 width = gabi::fsubs_ppc(finalX, base);
    u32 flags = styleFlags;
    if (maximumWidth < width) maximumWidth = width;
    u32 verticalMode = flags & 0x300;
    f32 resultY = originalY;
    if (verticalMode != 0x100 && verticalMode != 0x200) {
        f32 currentY = y;
        resultY = gabi::fadds_ppc(currentY, verticalAdjustment);
    }
    processor = tagProcessor;
    y = resultY;
    tagMethods = tagMethodsOf<TextDrawTagMethods>(processor);
    gabi::call_ptr<void>(tagMethods->end, processor, state.get());
    return maximumWidth;
}

/* WWHD glue
ext 0286EBEC void textCreateGlyphRecord(nw::lyt::GlyphRecord*)
ext 0286E8B4 f32 textPlaceGlyphRecord(nw::lyt::GlyphWriter*, nw::lyt::GlyphRecord*)
ext 0286EC88 void textInitializeReader(nw::lyt::FontInterface*, TextCodeReader*, u32)
*/
