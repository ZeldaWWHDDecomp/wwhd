/* Extended text-layout state on the merged bitmap glyph/text writer prefix. */
#pragma once
#include "nw/lyt/GlyphWriter.h"
namespace nw { namespace lyt {
struct TextMeasureSettings { be<f32> widthLimit, characterSpacing, lineSpacing; };
struct FontLineMethods { u8 prefix[0x54]; be<u32> lineFeed; };
struct TextLayoutWriter : TextWriter {
    be<u32> styleFlags;
    gptr<WideTagProcessor> tagProcessor;
    static TextLayoutWriter* create(TextLayoutWriter* writer);
    void destroy(u32 flags);
    f32 lineAdvance();
    void measureBoundsMutable(TextBounds* bounds, const be<u16>* text, s32 length);
    void measureBounds(TextBounds* bounds, const be<u16>* text, s32 length);
    f32 measureWidth(const be<u16>* text, s32 length);
    u32 measureLine(TextBounds* bounds, gptr<const be<u16>>* cursor, s32 length);
    f32 measureFirstLineWidth(const be<u16>* text, s32 length);
    f32 alignTextOrigin(be<f32>* horizontal, be<f32>* vertical, const be<u16>* text, s32 length);
    f32 drawTextMutable(const be<u16>* text, s32 length);
    f32 drawText(const be<u16>* text, s32 length);
};
WWHD_SIZE(TextLayoutWriter, 0x4C);
WWHD_SIZE(TextMeasureSettings, 0xC);
WWHD_OFFSET(TextWriter, reserved, 0x34);
WWHD_OFFSET(TextLayoutWriter, styleFlags, 0x44);
WWHD_OFFSET(TextLayoutWriter, tagProcessor, 0x48);
WWHD_OFFSET(FontLineMethods, lineFeed, 0x54);
} }
void* textLayoutAllocate(u32 size);
void textLayoutDelete(void* storage);
nw::lyt::GlyphWriter* textCreateGlyphWriter(nw::lyt::GlyphWriter* writer);
void textDestroyGlyphWriter(nw::lyt::GlyphWriter* writer, u32 flags);

f32 textLayoutFloor(f32);
f32 textLayoutAscent(nw::lyt::GlyphWriter*);
