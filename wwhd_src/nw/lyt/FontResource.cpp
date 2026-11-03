/* Bitmap-font resource views and glyph-sheet adapters, reconstructed from WWHD. */
#include "nw/lyt/FontResource.h"
using namespace nw::lyt;
static u8 sInterfaceVtable;
static u8 sFontVtable;
static u8 sResourceVtable;
struct TextureFormats { be<u32> values[18]; };
static TextureFormats sTextureFormats;
static TextureFormats sTextureComponents;
/* 0286EBEC */
GlyphRecord* GlyphRecord::create(GlyphRecord* record) {
    if(!record) record=(GlyphRecord*)layoutAllocate(sizeof(GlyphRecord));
    if(record) record->flags=0;
    return record;
}
/* 0286EC28 */
FontInterface* FontInterface::create(FontInterface* font) {
    if(!font) font=(FontInterface*)layoutAllocate(sizeof(FontInterface));
    if(font) { font->sheetFlip=1;font->vtable=(FontVtable*)&sInterfaceVtable;font->kerningEnabled=1; }
    return font;
}
/* 0286EC74 */
void FontInterface::destroy(u32 flags) { if(this && (flags&1)) layoutDelete(this); }
/* 0286EC88 */
void FontInterface::makeCursor(CharacterCursor* cursor) {
    u32 kind=gabi::call_ptr<u32>(vtable->encoding,this);
    u32 decoder=0,end=0;
    if(kind==1) { decoder=gabi::guest_fn((const void*)layoutDecodeCharacter);end=0xFFFF; }
    if(!cursor) cursor=(CharacterCursor*)layoutAllocate(sizeof(CharacterCursor));
    if(cursor) { cursor->character=0;cursor->decoder=decoder;cursor->text=0;cursor->end=end; }
}
/* 0286ED28 */
void SheetDescriptor::configure(u32 ready,u32 pixels,u32 type,u32 w,u32 h,u32 layerCount,u32 componentMask) {
    initialized=ready;image=pixels;format=type;width=w;height=h;layers=layerCount;components=componentMask;
}
/* 0286ED48 */
ResourceFont* ResourceFont::create(ResourceFont* font) {
    if(!font) font=(ResourceFont*)layoutAllocate(sizeof(ResourceFont));
    if(font) { FontResource::create(font);font->vtable=(FontVtable*)&sResourceVtable; }
    return font;
}
/* 0286ED9C */
FontFile* ResourceFont::unload() { FontFile* result=file;if(result) result=detach();return result; }
/* 0286EDD0 */
void ResourceFont::destroy(u32 flags) {
    if(!this) return;
    vtable=(FontVtable*)&sResourceVtable;
    if(file) unload();
    FontResource::destroy(0);
    if(flags&1) layoutDelete(this);
}
/* 0286EE44 */
void* FontFile::findBlock(u32 tag) {
    FontBlock* block=gabi::at<FontBlock>(gabi::ea(this)+(u32)headerSize);
    for(u32 i=0;i<(u16)blockCount;++i) {
        if((u32)block->tag==tag) return block->payload;
        block=gabi::at<FontBlock>(gabi::ea(block)+(u32)block->size);
    }
    return nullptr;
}
/* 0286EE84 */
FontInfo* FontFile::relocate() {
    FontInfo* result=nullptr;
    FontBlock* block=gabi::at<FontBlock>(gabi::ea(this)+(u16)headerSize);
    for(u32 i=0;i<(u16)blockCount;++i) {
        switch((u32)block->tag) {
        case InformationBlock:
            block->payload[3]=(u32)block->payload[3]+gabi::ea(this);
            result=(FontInfo*)block->payload;
            if(block->payload[4]) block->payload[4]=(u32)block->payload[4]+gabi::ea(this);
            if(block->payload[5]) block->payload[5]=(u32)block->payload[5]+gabi::ea(this);
            break;
        case SheetBlock:block->payload[5]=(u32)block->payload[5]+gabi::ea(this);break;
        case WidthsBlock:if(block->payload[1]) block->payload[1]=(u32)block->payload[1]+gabi::ea(this);break;
        case MapBlock:if(block->payload[2]) block->payload[2]=(u32)block->payload[2]+gabi::ea(this);break;
        case GlyphGroupsBlock:case KerningBlock:break;
        default:return nullptr;
        }
        block=gabi::at<FontBlock>(gabi::ea(block)+(u32)block->size);
    }
    magic=RelocatedFont;
    return result;
}
/* 0286EFEC */
u32 ResourceFont::load(FontFile* resource) {
    if(file) return 0;
    FontInfo* font;
    if((u32)resource->magic==RelocatedFont) font=(FontInfo*)resource->findBlock(InformationBlock);
    else { if(!layoutValidateFont(resource,PackedFont,0x03000000,2)) return 0;font=resource->relocate(); }
    if(!font) return 0;
    be<u16>* pairs=(be<u16>*)resource->findBlock(KerningBlock);
    attach(resource,font,pairs);
    buildSheet();
    return 1;
}
/* 0286F0CC */
void FontResource::invalidateSheet() { if(descriptor.initialized) descriptor.initialized=0; }
/* 0286F0E8 */
void TextureObject::initialize(u32 w,u32 h,u32 d,u32 mip,u32 fmt,u32 dim,u32 channels) {
    dimension=dim;antiAlias=0;usage=1;components=channels;tile=0;swizzle=0;
    width=w;firstMip=0;format=fmt;firstSlice=0;height=h;mipLevels=mip;sliceCount=d;depth=d;viewMipLevels=mip;
    layoutCalculateSurface(this);layoutInitializeTexture(this);
}
/* 0286F160 */
void FontResource::prepareSheet(SheetDescriptor* sheet) {
    if(sheet->initialized) return;
    u32 w=sheet->width,h=sheet->height,type=sheet->format,layerCount=sheet->layers;
    sheet->initialized=1;
    sheet->texture.initialize(w,h,layerCount,1,sTextureFormats.values[type],5,sTextureComponents.values[type]);
    u32 image=sheet->image;
    sheet->texture.image=image;
    u32 extra=0;
    if((u32)sheet->texture.mipLevels>1) extra=image+(u32)sheet->texture.mipOffsets[0];
    sheet->texture.mipmaps=extra;
}
/* 0286F204 */
void FontResource::buildSheet() {
    FontSheet* sheet=info->sheet;
    u32 layerCount=gabi::call_ptr<u32>(vtable->sheetLayers,this);
    descriptor.configure(0,sheet->image,sheet->format,sheet->width,sheet->height,layerCount&255,1);
    prepareSheet(&descriptor);
}
/* 0286F28C */
void FontResource::setWrap(u32 horizontal,u32 vertical) {
    wrap=(horizontal<<2)|(vertical<<1);
    if(file) { invalidateSheet();buildSheet(); }
}
/* 0286F2CC */
FontResource* FontResource::create(FontResource* font) {
    if(!font) font=(FontResource*)layoutAllocate(sizeof(FontResource));
    if(font) {
        FontInterface::create(font);
        font->info=nullptr;font->file=nullptr;font->vtable=(FontVtable*)&sFontVtable;
        font->descriptor.configure(0,0,12,0,0,0,1);
        font->descriptor.texture.image=0;font->kerning=nullptr;
        font->setWrap(1,1);
    }
    return font;
}
/* 0286F370 */
void FontResource::destroy(u32 flags) {
    if(!this) return;
    descriptor.configure(0,0,12,0,0,0,1);
    FontInterface::destroy(0);
    if(flags&1) layoutDelete(this);
}
/* 0286F3E8 */
u32 FontResource::width() { return info->width; }
/* 0286F3F4 */
u32 FontResource::height() { return info->height; }
/* 0286F400 */
u32 FontResource::ascent() { return info->ascent; }
/* 0286F40C */
s32 FontResource::lineFeed() { return info->lineFeed; }
/* 0286F418 */
u32 FontResource::lookupMap(const CharacterMap* map,u32 character) {
    switch((u16)map->method) {
    case 0:return (character-(u16)map->first+(u16)map->values[0])&0xFFFF;
    case 1:return map->values[character-(u16)map->first];
    case 2: {
        u32 low=gabi::ea(&map->values[1]),high=gabi::ea(map)+10+(u16)map->values[0]*4;
        while(low<=high) {
            u32 middle=low+((s32)(high-low)/8)*4;
            be<u16>* entry=gabi::at<be<u16>>(middle);
            u32 key=entry[0];
            if(character==key) return entry[1];
            if(character<key) high=middle-4;else low=middle+4;
        }
        break;
    }
    }
    return 0xFFFF;
}
/* 0286F4D8 */
u32 FontResource::findCharacter(u32 character) {
    CharacterMap* map=info->maps;
    while(map) {
        if((u16)map->first<=character && character<=(u16)map->last) return lookupMap(map,character);
        map=map->next;
    }
    return 0xFFFF;
}
/* 0286F520 */
u32 FontResource::setReplacement(u32 character) {
    u32 index=findCharacter(character);
    if(index==0xFFFF) return 0;
    info->replacement=index;return 1;
}
/* 0286F57C */
u32 FontResource::defaultAdvance(u32 character) { return (gabi::call_ptr<u32>(vtable->defaultWidth,this,character)>>8)&255; }
/* 0286F5C0 */
u32 FontResource::characterOrReplacement(u32 character) {
    u32 index=findCharacter(character);
    if(index==0xFFFF) index=info->replacement;
    return index;
}
/* 0286F5FC */
GlyphWidth* FontResource::widthInRange(WidthRange* range,u32 index) { return &range->entries[index-(u16)range->first]; }
/* 0286F614 */
GlyphWidth* FontResource::findWidth(u32 index) {
    FontInfo* font=info;
    WidthRange* range=font->widths;
    while(range) {
        if((u16)range->first<=index && index<=(u16)range->last) return widthInRange(range,index);
        range=range->next;
    }
    return &font->defaultWidth;
}
/* 0286F658 */
u32 FontResource::characterWidth(u32 character) {
    gabi::NativeFrame<0x18> frame;
    u32 index=characterOrReplacement(character);
    GlyphWidth* width=findWidth(index);
    struct PackedWidth { be<u8> left,width,advance,padding; };
    PackedWidth* packed=gabi::at<PackedWidth>(frame.sp()+12);
    packed->left=(u8)width->left;packed->width=width->width;packed->advance=width->advance;
    return ((u32)packed->left<<24)|((u32)packed->width<<16)|((u32)packed->advance<<8)|(u8)packed->padding;
}
/* 0286F6AC */
void GlyphRecord::locate(u32 index,const FontSheet* font) {
    u32 columns=font->columns,rows=font->rows;
    u32 cellWidth=font->cellWidth,cellHeight=font->cellHeight;
    u32 cells=columns*rows;
    u32 remainder=index-(index/cells)*cells;
    u32 row=remainder/columns;
    height=cellHeight;format=font->format;sheetWidth=font->width;
    u32 horizontal=(remainder-row*columns)*(cellWidth+1);
    sheetHeight=font->height;y=row*(cellHeight+1)+1;x=horizontal+1;
}
/* 0286F718 */
void FontResource::glyphByIndex(GlyphRecord* record,u32 index) {
    FontSheet* sheet=info->sheet;
    u32 page=index/((u32)sheet->columns*(u32)sheet->rows);
    record->image=(u32)sheet->image+page*(u32)sheet->sheetSize;
    GlyphWidth* width=findWidth(index);
    record->metrics.left=(s8)width->left;record->metrics.width=width->width;record->metrics.advance=width->advance;
    record->sheet=page;record->texture=&descriptor;
    record->locate(index,sheet);
}
/* 0286F7BC */
void FontResource::getGlyph(GlyphRecord* record,u32 character) { u32 index=characterOrReplacement(character);glyphByIndex(record,index); }
/* 0286F808 */
s32 FontResource::pairAdjustment(u32 left,u32 right) {
    be<u16>* table=kerning;
    if(!table || !(u8)kerningEnabled) return 0;
    u32 low=0,high=table[0],entry=0;
    while(true) {
        u32 middle=(low+high)/2;
        u32 key=table[1+middle*2];
        if(key==left) { entry=middle;break; }
        if(key>left) { if(high==middle) return 0;high=middle; }
        else { if(low==middle) return 0;low=middle; }
    }
    be<u16>* pairs=table+(u16)table[2+entry*2];
    low=0;high=pairs[0];
    while(true) {
        u32 middle=(low+high)/2,key=pairs[1+middle*2];
        if(key==right) return (s16)(u16)pairs[2+middle*2];
        if(key>right) { if(high==middle) return 0;high=middle; }
        else { if(low==middle) return 0;low=middle; }
    }
}
/* 0286F8D4 */
u32 FontResource::encoding() { return info->encoding; }
/* 0286F8E0 */
s32 FontResource::lineTop() { return info->sheet->lineTop; }
/* 0286F8F0 */
void FontResource::setComponents(u32 components) { descriptor.components=components; }
/* 0286F8F8 */
u32 FontResource::isWide() { return (u8)info->kind==2; }
/* 0286F910 */
void FontResource::attach(FontFile* data,FontInfo* font,be<u16>* pairs) { info=font;kerning=pairs;file=data; }
/* 0286F920 */
FontFile* FontResource::detach() { invalidateSheet();FontFile* result=file;file=nullptr;info=nullptr;kerning=nullptr;return result; }
/* 0286F958 */
u32 FontResource::sheetLayers() { return info->sheet->layers; }
/* WWHD glue
ext 0273AD10 void* layoutAllocate(u32)
ext 0273AF40 void layoutDelete(void*)
ext 02871158 u32 layoutValidateFont(nw::lyt::FontFile*,u32,u32,u32)
ext 02871328 u32 layoutDecodeCharacter(nw::lyt::CharacterCursor*) callback
ext C00060D0 void layoutCalculateSurface(nw::lyt::TextureObject*)
ext C0006498 void layoutInitializeTexture(nw::lyt::TextureObject*)
data sInterfaceVtable 1017FA34
data sFontVtable 1017FCBC
data sResourceVtable 1017FB2C
data sTextureFormats 1017FC2C
data sTextureComponents 1017FC74
*/
