#pragma once
#include "wwhd.h"
namespace nw { namespace lyt {
enum FontBlockTag : u32 { InformationBlock=0x46494E46,SheetBlock=0x54474C50,WidthsBlock=0x43574448,MapBlock=0x434D4150,GlyphGroupsBlock=0x474C4752,KerningBlock=0x4B524E47 };
enum FontFileMagic : u32 { PackedFont=0x46464E54,RelocatedFont=0x46464E55 };
struct GlyphWidth { be<s8> left;be<u8> width,advance; };
struct FontSheet;
struct GlyphRecord {
    be<u32> image;
    GlyphWidth metrics;be<u8> height;
    be<u16> sheetWidth,sheetHeight,x,y;
    be<u16> format;be<u8> flags,sheet;
    gptr<void> texture;
    static GlyphRecord* create(GlyphRecord*);
    void locate(u32,const FontSheet*);
};
struct WidthRange { be<u16> first,last;gptr<WidthRange> next;GlyphWidth entries[1]; };
struct CharacterMap { be<u16> first,last,method,reserved;gptr<CharacterMap> next;be<u16> values[1]; };
struct FontSheet {
    be<u8> cellWidth,cellHeight,layers,reserved;
    be<u32> sheetSize;be<s16> lineTop;be<u16> format,columns,rows,width,height;
    be<u32> image;
};
struct FontInfo {
    be<u8> kind,height,width,ascent;be<s16> lineFeed;be<u16> replacement;
    GlyphWidth defaultWidth;be<u8> encoding;
    gptr<FontSheet> sheet;gptr<WidthRange> widths;gptr<CharacterMap> maps;
};
struct FontVtable {
    u8 prefix[0x1C];be<u32> width;u8 widthPadding[4];be<u32> height;u8 heightPadding[4];be<u32> ascent;
    u8 metricsPadding[0x54];be<u32> defaultWidth;u8 glyphPadding[4];be<u32> glyph;
    u8 encodingPadding[0x14];be<u32> encoding;u8 transformPadding[0x4C];be<u32> transformed;
    u8 formatPadding[4];be<u32> sheetLayers;
};
struct CharacterCursor { be<u32> text;be<u16> character,end;be<u32> decoder; };
struct FontInterface {
    be<u8> kerningEnabled,sheetFlip;u8 padding[2];gptr<FontVtable> vtable;
    static FontInterface* create(FontInterface*);
    void destroy(u32);
    void makeCursor(CharacterCursor*);
};
struct FontFile {
    be<u32> magic;be<u16> byteOrder,headerSize;be<u32> version,size;be<u16> blockCount,reserved;
    void* findBlock(u32);
    FontInfo* relocate();
};
struct FontBlock { be<u32> tag,size;be<u32> payload[6]; };
struct TextureObject {
    be<u32> dimension,width,height,depth,mipLevels,format,antiAlias,usage,imageSize,image,mipSize,mipmaps;
    be<u32> tile,swizzle,alignment,pitch,mipOffsets[13];
    be<u32> firstMip,viewMipLevels,firstSlice,sliceCount,components,registers[5];
    void initialize(u32,u32,u32,u32,u32,u32,u32);
};
struct SheetDescriptor {
    be<u32> initialized,image;be<u16> height,width;be<u8> format,layers,components,reserved;
    TextureObject texture;
    void configure(u32,u32,u32,u32,u32,u32,u32);
};
struct FontResource : FontInterface {
    gptr<FontFile> file;gptr<FontInfo> info;SheetDescriptor descriptor;
    be<u32> wrap;gptr<be<u16>> kerning;
    static FontResource* create(FontResource*);
    void destroy(u32);void invalidateSheet();void buildSheet();void prepareSheet(SheetDescriptor*);void setWrap(u32,u32);
    u32 width();u32 height();u32 ascent();s32 lineFeed();
    u32 lookupMap(const CharacterMap*,u32);u32 findCharacter(u32);u32 setReplacement(u32);u32 characterOrReplacement(u32);
    u32 defaultAdvance(u32);GlyphWidth* widthInRange(WidthRange*,u32);GlyphWidth* findWidth(u32);u32 characterWidth(u32);
    void glyphByIndex(GlyphRecord*,u32);void getGlyph(GlyphRecord*,u32);s32 pairAdjustment(u32,u32);
    u32 encoding();s32 lineTop();void setComponents(u32);u32 isWide();void attach(FontFile*,FontInfo*,be<u16>*);FontFile* detach();u32 sheetLayers();
};
struct ResourceFont : FontResource {
    static ResourceFont* create(ResourceFont*);FontFile* unload();void destroy(u32);u32 load(FontFile*);
};
WWHD_SIZE(GlyphWidth,3);WWHD_SIZE(GlyphRecord,0x18);WWHD_OFFSET(GlyphRecord,texture,0x14);
WWHD_OFFSET(FontVtable,width,0x1C);WWHD_OFFSET(FontVtable,height,0x24);WWHD_OFFSET(FontVtable,defaultWidth,0x84);WWHD_OFFSET(FontVtable,glyph,0x8C);WWHD_OFFSET(FontVtable,encoding,0xA4);WWHD_OFFSET(FontVtable,transformed,0xF4);WWHD_OFFSET(FontVtable,sheetLayers,0xFC);
WWHD_SIZE(FontInterface,8);WWHD_SIZE(FontInfo,0x18);WWHD_SIZE(FontSheet,0x18);WWHD_SIZE(CharacterCursor,0xC);WWHD_SIZE(TextureObject,0x9C);WWHD_SIZE(SheetDescriptor,0xAC);WWHD_SIZE(FontResource,0xC4);
WWHD_OFFSET(FontResource,info,0xC);WWHD_OFFSET(FontResource,descriptor,0x10);WWHD_OFFSET(FontResource,wrap,0xBC);WWHD_OFFSET(FontResource,kerning,0xC0);
} }
void* layoutAllocate(u32);void layoutDelete(void*);u32 layoutValidateFont(nw::lyt::FontFile*,u32,u32,u32);
u32 layoutDecodeCharacter(nw::lyt::CharacterCursor*);
void layoutCalculateSurface(nw::lyt::TextureObject*);void layoutInitializeTexture(nw::lyt::TextureObject*);
