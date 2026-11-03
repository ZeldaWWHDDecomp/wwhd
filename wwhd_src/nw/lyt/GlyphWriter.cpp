/* Glyph-quad writing and text control tags, reconstructed from WWHD. */
#include "nw/lyt/GlyphWriter.h"
using namespace nw::lyt;
using StateWord = be<u32>;
static StateWord sWriterGuard;
static StateWord sWriterState;
static StateWord sTagGuard;
static StateWord sWideTagGuard;
static TagProcessor sDefaultTags;
static WideTagProcessor sDefaultWideTags;
struct GlobalRegistration { be<u32> words[3]; };
static GlobalRegistration sTagRegistration;
static GlobalRegistration sWideTagRegistration;
static u8 sTagVtable;
static u8 sWideTagVtable;
/* 0286E20C */
GlyphWriter* GlyphWriter::create(GlyphWriter* writer) {
    if(!writer) writer=(GlyphWriter*)writerAllocate(sizeof(GlyphWriter));
    if(writer) {
        layoutConstructColors(writer,2,4,gabi::guest_fn((const void*)ColorWord::create));
        writer->batch=nullptr;writer->advance=0;writer->bearing=0;writer->fixedAdvance=0;writer->font=nullptr;writer->colorFlags=0;
        writer->colors[0].value=0xFFFFFFFF;writer->colors[1].value=0xFFFFFFFF;
        writer->x=0;writer->scaleX=1;writer->y=0;writer->z=0;writer->scaleY=1;
    }
    return writer;
}
/* 0286E2D0 */
void GlyphWriter::destroy(u32 flags) { if(this && (flags&1)) writerDelete(this); }
/* 0286E2E4 */
void GlyphWriter::setSize(f32 width,f32 height) {
    FontInterface* current=font;
    s32 nativeWidth=gabi::call_ptr<s32>(current->vtable->width,current);
    f32 horizontal=width/(f32)nativeWidth;
    current=font;
    s32 nativeHeight=gabi::call_ptr<s32>(current->vtable->height,current);
    scaleX=horizontal;scaleY=height/(f32)nativeHeight;
}
/* 0286E3C4 */
f32 GlyphWriter::scaledWidth() { FontInterface* current=font;s32 width=gabi::call_ptr<s32>(current->vtable->width,current);return (f32)width*(f32)scaleX; }
/* 0286E42C */
f32 GlyphWriter::scaledHeight() { FontInterface* current=font;s32 height=gabi::call_ptr<s32>(current->vtable->height,current);return (f32)height*(f32)scaleY; }
/* 0286E494 */
f32 GlyphWriter::scaledAscent() { FontInterface* current=font;s32 ascent=gabi::call_ptr<s32>(current->vtable->ascent,current);return (f32)ascent*(f32)scaleY; }
/* 0286E4FC */
void GlyphWriter::appendGlyph(const GlyphRecord* glyph,f32 horizontal) {
    FontInterface* current=font;
    f32 denominatorX=(u16)glyph->sheetWidth,denominatorY=(u16)glyph->sheetHeight;
    f32 left,right,top,bottom,width,height,vertical;
    if((u8)current->sheetFlip) {
        right=((f32)((u32)(u16)glyph->x+(u8)glyph->metrics.width)+0.5f)/denominatorX;
        left=((f32)(u16)glyph->x-0.5f)/denominatorX;
        top=1.0f-((f32)(u16)glyph->y-0.5f)/denominatorY;
        bottom=1.0f-((f32)((u32)(u16)glyph->y+(u8)glyph->height)+0.5f)/denominatorY;
        f32 sx=scaleX,sy=scaleY;
        horizontal=gabi::fnmsubs(0.5f,sx,horizontal);
        vertical=gabi::fnmsubs(0.5f,sy,(f32)y);
        width=((f32)(u8)glyph->metrics.width+1.0f)*sx;height=((f32)(u8)glyph->height+1.0f)*sy;
    } else {
        right=(f32)((u32)(u16)glyph->x+(u8)glyph->metrics.width)/denominatorX;
        left=(f32)(u16)glyph->x/denominatorX;
        top=1.0f-(f32)(u16)glyph->y/denominatorY;
        bottom=1.0f-(f32)((u32)(u16)glyph->y+(u8)glyph->height)/denominatorY;
        width=(f32)(u8)glyph->metrics.width*(f32)scaleX;height=(f32)(u8)glyph->height*(f32)scaleY;vertical=y;
    }
    GlyphBatch_l* output=batch;
    u32 count=output->count;
    if(count>=(u32)output->capacity) return;
    output->count=count+1;
    output=batch;
    GlyphQuad* quad=&output->quads[count];
    quad->width=width;quad->height=height;quad->x=horizontal;quad->y=vertical;
    quad->topColor.value=colors[0].value;quad->bottomColor.value=colors[1].value;
    quad->left=left;quad->top=top;quad->right=right;quad->bottom=bottom;
    current=font;
    f32 offset=(f32)bearing*(f32)scaleX;
    s32 nativeWidth=gabi::call_ptr<s32>(current->vtable->width,current);
    quad->bearing=(s16)gabi::ftoi(offset*(f32)nativeWidth);
    quad->layer=glyph->sheet;quad->flags=0;quad->texture=glyph->texture;
    current=font;
    u32 transformed=gabi::call_ptr<u32>(current->vtable->transformed,current);
    if(transformed) quad->flags=(u8)quad->flags|1;
}
/* 0286E8B4 */
f32 GlyphWriter::placeGlyph(const GlyphRecord* glyph) {
    f32 sx=scaleX,step;
    if((u8)fixedAdvance) {
        step=advance;
        f32 bearing=(f32)(s8)glyph->metrics.left*sx;
        f32 centered=gabi::fnmsubs((f32)(u8)glyph->metrics.advance,sx,step);
        f32 offset=gabi::fmadds(centered,0.5f,bearing);
        appendGlyph(glyph,(f32)x+offset);
    } else {
        step=(f32)(u8)glyph->metrics.advance*sx;
        appendGlyph(glyph,gabi::fmadds((f32)(s8)glyph->metrics.left,sx,(f32)x));
    }
    x=(f32)x+step;
    return step;
}
/* 0286E9F0 */
f32 GlyphWriter::printCharacter(u32 character) {
    gabi::NativeFrame<0x28> frame;
    GlyphRecord* record=gabi::at<GlyphRecord>(frame.sp()+8);
    GlyphRecord::create(record);
    FontInterface* current=font;
    gabi::call_ptr<void>(current->vtable->glyph,current,record,character);
    return placeGlyph(record);
}
/* 0286EA54 */
u32 GlyphWriter::batchBytes(u32 count) { return count*0x30+((((count+7)>>3)+3)&~3u)+0xA4; }
/* 0286EA74 */
u32 GlyphWriter::vertexBytes(u32 count) { return count*0xE0; }
/* 0286EA7C */
GlyphBatch_l* GlyphWriter::initializeBatch(GlyphBatch_l* memory,u32 vertices,u32 count) { if(memory) return GlyphBatch_l::create(memory,count,vertices);return memory; }
/* 0286EAB0 */
void GlyphWriter::clearBatch() { batch->count=0;batch->dirty=0; }
/* 0286EAC8 */
void GlyphWriter::finishBatch() {}
/* 0286EACC */
ColorWord* ColorWord::create(ColorWord* color) { if(!color) color=(ColorWord*)writerAllocate(sizeof(ColorWord));if(color) color->value=0xFFFFFFFF;return color; }
/* 0286EB08 */
GlyphBatch_l* GlyphBatch_l::create(GlyphBatch_l* memory,u32 count,u32 vertexMemory) {
    if(!memory) memory=(GlyphBatch_l*)writerAllocate(0xA4);
    if(memory) {
        memory->capacity=count;memory->commands=0;memory->indices=0;memory->dirty=0;memory->count=0;
        u32 address=gabi::ea(memory)+0x14;
        struct VertexPair { be<u32> words[3]; };
        VertexPair* buffers=gabi::at<VertexPair>(address);
        if(!address) buffers=(VertexPair*)writerAllocate(sizeof(VertexPair));
        if(buffers) { buffers->words[2]=0;buffers->words[0]=0;buffers->words[1]=0; }
        memory->groupCount=0;
        memory->trailingBits=gabi::ea(memory)+0xA4+(u32)memory->capacity*0x30;
        memory->vertices[0]=vertexMemory;memory->vertices[1]=vertexMemory+(count*4)*0x1C;
    }
    return memory;
}
/* 0286F968 */
be<u32>* nw::lyt::initializeWriterState() { if(!(u32)sWriterGuard) { sWriterGuard=1;sWriterState=0; }return &sWriterState; }
/* 0286F994 */
void nw::lyt::beginText() {}
/* 0286F998 */
void nw::lyt::endText() {}
/* 0286F99C */
TagProcessor* TagProcessor::create(TagProcessor* processor) { if(!processor) processor=(TagProcessor*)writerAllocate(sizeof(TagProcessor));if(processor) processor->vtable=gabi::ea(&sTagVtable);return processor; }
/* 0286F9DC */
void TagProcessor::destroy(u32 flags) { if(this && (flags&1)) writerDelete(this); }
/* 0286F9F0 */
WideTagProcessor* WideTagProcessor::create(WideTagProcessor* processor) { if(!processor) processor=(WideTagProcessor*)writerAllocate(sizeof(WideTagProcessor));if(processor) processor->vtable=gabi::ea(&sWideTagVtable);return processor; }
/* 0286FA30 */
void WideTagProcessor::destroy(u32 flags) { if(this && (flags&1)) writerDelete(this); }
/* 0286FA44 */
void TagProcessor::newLine(TextContext* context) {
    TextWriter* writer=context->writer;
    f32 origin=context->lineOrigin;
    f32 step=layoutLineStep(writer);
    f32 next=(f32)writer->y+step;
    writer->x=origin;writer->y=next;
}
/* 0286FAA0 */
void TagProcessor::tab(TextContext* context) {
    TextWriter* writer=context->writer;
    s32 count=writer->tabColumns;
    if(count<=0) return;
    f32 width;
    if((u8)writer->fixedAdvance) width=writer->advance;else width=writer->scaledWidth();
    f32 period=(f32)count*width;
    f32 origin=context->lineOrigin;
    f32 previous=(f32)writer->x-origin;
    s32 column=gabi::ftoi(previous/period);
    f32 next=(f32)(s32)((u32)column+1);
    writer->x=gabi::fmadds(period,next,origin);
}
/* 0286FBDC */
u32 TagProcessor::process(u32 character,TextContext* context) {
    switch(character) { case 10:newLine(context);return 3;case 9:tab(context);return 1;default:return 0; }
}
/* 0286FC44 */
u32 TagProcessor::measure(TextBounds* bounds,u32 character,TextContext* context) {
    if(character!=9 && character!=10) return 0;
    TextWriter* writer=context->writer;
    f32 lastY;
    if(character==10) {
        bounds->right=writer->x;bounds->top=writer->y;
        newLine(context);bounds->left=writer->x;
        TextWriter* current=context->writer;
        f32 height=current->scaledHeight();
        lastY=(f32)writer->y+height;
    } else {
        bounds->left=writer->x;tab(context);bounds->right=writer->x;
        f32 y=writer->y;bounds->top=y;lastY=y+writer->scaledHeight();
    }
    f32 left=bounds->left,right=bounds->right,top=bounds->top;
    f32 horizontal=right-left,vertical=lastY-top;
    bounds->left=horizontal>=0?left:right;bounds->right=horizontal>=0?right:left;
    bounds->top=vertical>=0?top:lastY;bounds->bottom=vertical>=0?lastY:top;
    return character==10?3:1;
}
/* 0286FDD4 */
void nw::lyt::initializeTagProcessors() {
    if(!(u32)sTagGuard) { sTagGuard=1;TagProcessor::create(&sDefaultTags);layoutRegisterObject(&sTagRegistration); }
    if(!(u32)sWideTagGuard) { sWideTagGuard=1;WideTagProcessor::create(&sDefaultWideTags);layoutRegisterObject(&sWideTagRegistration); }
}
/* WWHD glue
ext 0273AD10 void* writerAllocate(u32)
ext 0273AF40 void writerDelete(void*)
ext 028EFFD0 void layoutConstructColors(void*,u32,u32,u32)
ext 0286FF24 f32 layoutLineStep(nw::lyt::TextWriter*)
ext 028F026C void layoutRegisterObject(void*)
data sWriterGuard 101FDD5C
data sWriterState 101FDD60
data sTagGuard 101FE060
data sWideTagGuard 101FE064
data sDefaultTags 104B6840
data sDefaultWideTags 104B6844
data sTagRegistration 101FC89C
data sWideTagRegistration 101FC8A8
data sTagVtable 1017FDCC
data sWideTagVtable 1017FE04
*/
