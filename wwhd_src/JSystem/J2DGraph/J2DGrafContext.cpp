/* 2D graphics context, following tww J2DGrafContext.
 * HD: GX submission is omitted; retained state uses the Cafe tail vtable. */
#include "JSystem/J2DGraph/J2DGrafContext.h"
static f32 sZero=0.0f;
static f32 sMaxWidth=1024.0f;
static f32 sMaxHeight=1000.0f;
struct StaticState { be<u32> words[4]; };
static StaticState sStaticState;
static u8 sStaticRecord;
static J2DGrafVtable sBaseVtable;
J2DGrafVtable* J2DGetBaseVtable() { return &sBaseVtable; }

/* 027EDA4C */
void J2DGrafContext::setColor(const J2DColor* tl,const J2DColor* tr,const J2DColor* br,const J2DColor* bl) {
    u32 topRight=tr->rgba,topLeft=tl->rgba,bottomRight=br->rgba,bottomLeft=bl->rgba;
    mColorTL.rgba=topLeft; mColorTR.rgba=topRight; mColorBR.rgba=bottomRight; mColorBL.rgba=bottomLeft;
    u8 alpha=mColorTL.rgba & 255;
    mLinePart.mType=1; mBlend.mSrcFactor=4; mBoxPart.mType=1; mBlend.mDstFactor=5;
    mLinePart.mSrcFactor=4; mLinePart.mDstFactor=5; mBoxPart.mSrcFactor=4; mBlend.mType=1; mBoxPart.mDstFactor=5;
    if(alpha!=255) return;
    alpha=mColorBR.rgba & 255;
    mBlend.mDstFactor=0; mBlend.mType=0; mBlend.mSrcFactor=1;
    if(alpha!=255) return;
    alpha=mColorTR.rgba & 255;
    mLinePart.mDstFactor=0; mLinePart.mSrcFactor=1; mLinePart.mType=0;
    if(alpha!=255 || (mColorBL.rgba & 255)!=255) return;
    mBoxPart.mDstFactor=0; mBoxPart.mType=0; mBoxPart.mSrcFactor=1;
}
/* 027EDB4C */
void J2DGrafContext::setLineWidth(u8 width) { mLineWidth=width; }
/* 027EDB54 */
J2DGrafContext* J2DGrafContext__ct(J2DGrafContext* context,f32 x,f32 y,f32 width,f32 height) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if(!context) context=static_cast<J2DGrafContext*>(grafAllocate(sizeof(J2DGrafContext)));
    if(!context) return nullptr;
    f32 right=gabi::fadds_ppc(x,width),bottom=gabi::fadds_ppc(y,height);
    context->__vtbl=J2DGetBaseVtable();
    context->mBounds.i.y=y; context->mBounds.f.x=right; context->mBounds.i.x=x; context->mBounds.f.y=bottom;
    J2DBox* scissor=&context->mScissorBounds;
    if(gabi::ea(scissor)==0) scissor=static_cast<J2DBox*>(grafAllocate(sizeof(J2DBox)));
    if(scissor) { scissor->i.y=y; scissor->f.x=right; scissor->i.x=x; scissor->f.y=bottom; }
    context->mColorBL.rgba=0xFFFFFFFF; context->mColorBR.rgba=0xFFFFFFFF;
    context->mColorTR.rgba=0xFFFFFFFF; context->mColorTL.rgba=0xFFFFFFFF;
    if(gabi::ea(&context->mPrevPos)==0) grafAllocate(sizeof(J2DPoint));
    gabi::FrameLocal<J2DColor> tl(0xC),tr(0x10),br(0x14),bl(0x18);
    tl->rgba=0xFFFFFFFF;tr->rgba=0xFFFFFFFF;br->rgba=0xFFFFFFFF;bl->rgba=0xFFFFFFFF;
    context->setColor(tl.get(),tr.get(),br.get(),bl.get());
    context->setLineWidth(6);
    return context;
}
static void copyBox(J2DBox* dst,const J2DBox* src) {
    dst->i.xBits=static_cast<u32>(src->i.xBits); dst->i.yBits=static_cast<u32>(src->i.yBits);
    dst->f.xBits=static_cast<u32>(src->f.xBits); dst->f.yBits=static_cast<u32>(src->f.yBits);
}
/* 027EDCE8 */
void J2DGrafContext::place(const J2DBox* box) { copyBox(&mBounds,box); copyBox(&mScissorBounds,box); }
/* 027EDD2C */
void J2DGrafContext::scissor(const J2DBox* box) { copyBox(&mScissorBounds,box); }
/* 027EDD50 */
void J2DGrafContext::setPort() {
    gabi::call_ptr<void>(__vtbl->setScissor,this);
    gabi::call_ptr<void>(__vtbl->setup2D,this);
}
/* 027EDD9C */
void J2DGrafContext::setScissor() {
    f32 left=mScissorBounds.i.x,zero=sZero,top=mScissorBounds.i.y;
    if(!(left>zero)) { top=mScissorBounds.i.y; mScissorBounds.i.x=zero; }
    if(!(top>zero)) mScissorBounds.i.y=zero;
    f32 right=mScissorBounds.f.x,maxWidth=sMaxWidth;
    if(!(right<maxWidth)) mScissorBounds.f.x=maxWidth;
    f32 bottom=mScissorBounds.f.y,maxHeight=sMaxHeight;
    if(!(bottom<maxHeight)) mScissorBounds.f.y=maxHeight;
}
/* 027EDDFC */
static void __sinit_J2DGrafContext_cpp() {
    sStaticState.words[2]=0; sStaticState.words[0]=0; sStaticState.words[3]=0; sStaticState.words[1]=0;
    grafRegisterStatic(&sStaticRecord);
}
/* 027EDE24 */
void J2DGrafContext::setup2D() {}
/* 027EDE28 */
u32 J2DGrafContext::getGrafType() { return 0; }
/* WWHD glue
data sBaseVtable 1016DE54
data sZero 1016DE40
data sMaxWidth 1016DE44
data sMaxHeight 1016DE48
data sStaticState 104B4518
data sStaticRecord 101F982C
ext 0273AD10 void* grafAllocate(u32)
ext 028F026C void grafRegisterStatic(void*)
*/
