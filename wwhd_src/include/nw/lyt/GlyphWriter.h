#pragma once
#include "nw/lyt/FontResource.h"
namespace nw { namespace lyt {
struct ColorWord { be<u32> value;static ColorWord* create(ColorWord*); };
struct GlyphQuad {
    be<f32> width,height,x,y;ColorWord topColor,bottomColor;
    be<f32> left,top,right,bottom;gptr<void> texture;
    be<s16> bearing;be<u8> layer,flags;
};
struct GlyphGroup { be<u32> texture,first,count,flags; };
// Local batch view shared conceptually with the pending NW-UT renderer.
struct GlyphBatch_l {
    be<u32> capacity;be<u16> count;be<u8> dirty,reserved;
    be<u32> trailingBits,indices,commands,vertices[2],active;
    GlyphGroup groups[8];be<u32> groupCount;GlyphQuad quads[1];
    static GlyphBatch_l* create(GlyphBatch_l*,u32,u32);
};
struct GlyphWriter {
    ColorWord colors[2];be<u32> colorFlags;be<f32> scaleX,scaleY,x,y,z,advance,bearing;
    gptr<FontInterface> font;gptr<GlyphBatch_l> batch;be<u8> fixedAdvance;u8 padding[3];
    static GlyphWriter* create(GlyphWriter*);void destroy(u32);
    void setSize(f32,f32);f32 scaledWidth();f32 scaledHeight();f32 scaledAscent();
    void appendGlyph(const GlyphRecord*,f32);f32 placeGlyph(const GlyphRecord*);f32 printCharacter(u32);
    static u32 batchBytes(u32);static u32 vertexBytes(u32);static GlyphBatch_l* initializeBatch(GlyphBatch_l*,u32,u32);
    void clearBatch();void finishBatch();
};
struct TextWriter : GlyphWriter { u8 reserved[0xC];be<s32> tabColumns; };
struct TextContext { gptr<TextWriter> writer;be<u32> text,flags;be<f32> lineOrigin; };
struct TextBounds { be<f32> left,top,right,bottom; };
struct TagProcessor {
    be<u32> vtable;
    static TagProcessor* create(TagProcessor*);void destroy(u32);void newLine(TextContext*);void tab(TextContext*);u32 process(u32,TextContext*);u32 measure(TextBounds*,u32,TextContext*);
};
struct WideTagProcessor { be<u32> vtable;static WideTagProcessor* create(WideTagProcessor*);void destroy(u32); };
be<u32>* initializeWriterState();void beginText();void endText();void initializeTagProcessors();
WWHD_SIZE(ColorWord,4);WWHD_SIZE(GlyphQuad,0x30);WWHD_OFFSET(GlyphBatch_l,vertices,0x14);WWHD_OFFSET(GlyphBatch_l,quads,0xA4);WWHD_OFFSET(GlyphBatch_l,groupCount,0xA0);
WWHD_SIZE(GlyphWriter,0x34);WWHD_OFFSET(GlyphWriter,font,0x28);WWHD_OFFSET(GlyphWriter,fixedAdvance,0x30);WWHD_OFFSET(TextWriter,tabColumns,0x40);WWHD_SIZE(TextBounds,0x10);
} }
void layoutConstructColors(void*,u32,u32,u32);f32 layoutLineStep(nw::lyt::TextWriter*);void layoutRegisterObject(void*);
void* writerAllocate(u32);void writerDelete(void*);
