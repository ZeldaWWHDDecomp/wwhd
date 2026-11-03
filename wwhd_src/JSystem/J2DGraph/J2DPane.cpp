/* Pane state and hierarchy, following tww J2DPane with the Cafe tail vtable. */
#include "JSystem/J2DGraph/J2DPane.h"
static f32 sZero=0.0f;
static f32 sHalf=0.5f;
static f32 sDegreesToRadians=0.01745329238474369f;
struct PaneStaticState { be<u32> words[4]; };
static PaneStaticState sStaticState;
static u8 sStaticRecord;
static J2DPaneVtable sPaneVtable;
/* 027EE06C */
void J2DPane::initiate() {
    J2DPaneVtable* vtable=__vtbl;
    f32 zero=sZero;
    mCullMode=0;mRotation=zero;mBasePosition.y=zero;mInfluencedAlpha=1;
    mRotationAxis='z';mDrawAlpha=255;mIsConnectParent=0;mAlpha=255;mBasePosition.x=zero;
    gabi::call_ptr<void>(vtable->calcMtx,this);
}
static J2DPaneTree* constructPaneStorage(J2DPane* pane) {
    if(gabi::ea(&pane->mBounds)==0) paneAllocate(sizeof(J2DBox));
    if(gabi::ea(&pane->mGlobalBounds)==0) paneAllocate(sizeof(J2DBox));
    if(gabi::ea(&pane->mDrawBounds)==0) paneAllocate(sizeof(J2DBox));
    J2DPaneTree* tree=&pane->mPaneTree;
    if(gabi::ea(tree)==0) tree=static_cast<J2DPaneTree*>(paneAllocate(sizeof(J2DPaneTree)));
    if(tree) { paneListInit(&tree->children);paneLinkConstruct(&tree->link,pane); }
    return tree;
}
/* 027EE0B8 */
J2DPane* J2DPane__ct(J2DPane* pane) {
    if(!pane) pane=static_cast<J2DPane*>(paneAllocate(sizeof(J2DPane)));
    if(!pane) return nullptr;
    pane->__vtbl=&sPaneVtable;
    constructPaneStorage(pane);
    f32 zero=sZero;
    pane->mMagic=0x50414E31;pane->mVisible=1;pane->mTag=0;
    pane->mBounds.i.x=zero;pane->mBounds.f.x=zero;pane->mBounds.f.y=zero;pane->mBounds.i.y=zero;
    pane->initiate();return pane;
}
/* 027EE1A4 */
void J2DPane::setBasePosition(s32 position) {
    f32 half=sHalf,zero=sZero;
    s32 row=position/3,column=position-row*3;
    m2DBasePosition=position;
    if(column==0) mBasePosition.x=zero;
    else {
        f32 right=mBounds.f.x,left=mBounds.i.x;
        f32 width=gabi::fsubs_ppc(right,left);
        if(column==1) width=gabi::fmuls_ppc(width,half);
        mBasePosition.x=width;
    }
    f32 y=zero;
    if(row!=0) { f32 bottom=mBounds.f.y,top=mBounds.i.y;y=gabi::fsubs_ppc(bottom,top);if(row==1) y=gabi::fmuls_ppc(y,half); }
    mBasePosition.y=y;mRotationAxis='z';
    gabi::call_ptr<void>(__vtbl->calcMtx,this);
}
struct PaneStreamFrame {
    be<u8> size,alpha,influenced,position;
    be<u16> rotation;be<s16> x;
    u8 conversion[8];be<s16> y,width,height;u8 padding[2];be<u32> tag;
};
/* 027EE29C */
void J2DPane::makePaneStream(J2DPane* parent,JSURandomInputStream* stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<PaneStreamFrame> frame(8);
    paneRead(stream,&frame->size,1);
    paneRead(stream,&mVisible,1);
    gabi::call_ptr<s32>(stream->mVtable->mSkip[1],stream,2);
    paneRead(stream,&frame->tag,4);mTag=static_cast<u32>(frame->tag);
    paneRead(stream,&frame->x,2);f32 x=static_cast<s16>(frame->x);
    paneRead(stream,&frame->y,2);f32 y=static_cast<s16>(frame->y);
    paneRead(stream,&frame->width,2);f32 right=gabi::fadds_ppc(static_cast<s16>(frame->width),x);
    paneRead(stream,&frame->height,2);f32 height=static_cast<s16>(frame->height);
    u8 remaining=static_cast<u8>(frame->size-6);
    mBounds.i.x=x;mBounds.i.y=y;mBounds.f.x=right;
    frame->size=remaining;mBounds.f.y=gabi::fadds_ppc(height,y);mRotation=sZero;
    bool havePosition=false;
    if(remaining) {
        paneRead(stream,&frame->rotation,2);
        remaining=static_cast<u8>(frame->size-1);
        mRotation=static_cast<u16>(frame->rotation);frame->size=remaining;
        if(remaining) {
            paneRead(stream,&frame->position,1);
            setBasePosition(frame->position);
            remaining=static_cast<u8>(frame->size-1);mAlpha=255;frame->size=remaining;
            havePosition=true;
        }
    }
    if(!havePosition) { setBasePosition(0);remaining=frame->size;mAlpha=255; }
    if(remaining) {
        paneRead(stream,&frame->alpha,1);
        remaining=static_cast<u8>(frame->size-1);mAlpha=static_cast<u8>(frame->alpha);frame->size=remaining;
    }
    mInfluencedAlpha=1;
    if(remaining) {
        paneRead(stream,&frame->influenced,1);
        u8 influenced=frame->influenced,rest=frame->size;
        frame->size=static_cast<u8>(rest-1);mInfluencedAlpha=influenced!=0;
    }
    paneAlign(stream,4);
    if(parent) {
        J2DPaneTree* tree=&mPaneTree;
        paneListAppend(&parent->mPaneTree.children,gabi::ea(tree)?&tree->link:nullptr);
    }
    J2DPaneVtable* vtable=__vtbl;
    mCullMode=0;mDrawAlpha=255;mIsConnectParent=0;
    gabi::call_ptr<void>(vtable->calcMtx,this);
}
/* 027EE5DC */
J2DPane* J2DPane__ct_parent(J2DPane* pane,J2DPane* parent,u32 visible,u32 tag,const J2DBox* bounds) {
    if(!pane) pane=static_cast<J2DPane*>(paneAllocate(sizeof(J2DPane)));
    if(!pane) return nullptr;
    pane->__vtbl=&sPaneVtable;
    J2DPaneTree* originalTree=&pane->mPaneTree;
    constructPaneStorage(pane);
    pane->mTag=tag;pane->mVisible=visible;pane->mMagic=0x50414E31;
    pane->mBounds.i.xBits=static_cast<u32>(bounds->i.xBits);pane->mBounds.i.yBits=static_cast<u32>(bounds->i.yBits);
    pane->mBounds.f.xBits=static_cast<u32>(bounds->f.xBits);pane->mBounds.f.yBits=static_cast<u32>(bounds->f.yBits);
    if(parent) paneListAppend(&parent->mPaneTree.children,gabi::ea(originalTree)?&originalTree->link:nullptr);
    pane->initiate();return pane;
}
/* 027EE6F0 */
static void J2DPane__dt(J2DPane* pane,u32 flags) {
    if(!pane) return;
    JSUPtrLink* child=pane->mPaneTree.children.mHead;
    pane->__vtbl=&sPaneVtable;
    while(child) {
        JSUPtrLink* next=child->mNext;
        J2DPane* object=static_cast<J2DPane*>(static_cast<void*>(child->mObject));
        child=next;
        if(object) gabi::call_ptr<void>(object->__vtbl->destroy,object,3);
    }
    paneLinkDestroy(&pane->mPaneTree.link,0);paneListDestroy(&pane->mPaneTree.children,0);
    if(flags&1) paneDelete(pane);
}
/* 027EE7BC */
void J2DPane::draw(f32 x,f32 y,const J2DGrafContext* context,u32 clip) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<J2DGrafContext> local(0x18);
    // HD: GHS copies the visible context state, then replaces the copied vtable with the base one.
    f32 bottom=context->mBounds.f.y,top=context->mBounds.i.y,right=context->mBounds.f.x;
    local->mBounds.i.y=top;local->mBounds.f.x=right;
    f32 scissorTop=context->mScissorBounds.i.y;
    local->mBounds.f.y=bottom;local->mScissorBounds.i.y=scissorTop;
    f32 scissorBottom=context->mScissorBounds.f.y,scissorLeft=context->mScissorBounds.i.x,left=context->mBounds.i.x;
    local->__vtbl=J2DGetBaseVtable();local->mBounds.i.x=left;
    local->mScissorBounds.i.x=scissorLeft;
    f32 scissorRight=context->mScissorBounds.f.x;
    local->mScissorBounds.f.y=scissorBottom;local->mScissorBounds.f.x=scissorRight;
    local->mColorTL.rgba=static_cast<u32>(context->mColorTL.rgba);local->mColorTR.rgba=static_cast<u32>(context->mColorTR.rgba);
    local->mColorBR.rgba=static_cast<u32>(context->mColorBR.rgba);local->mColorBL.rgba=static_cast<u32>(context->mColorBL.rgba);
    local->mLineWidth=static_cast<u8>(context->mLineWidth);
    local->mPrevPos.xBits=static_cast<u32>(context->mPrevPos.xBits);local->mPrevPos.yBits=static_cast<u32>(context->mPrevPos.yBits);
    paneCopyBytes(local->mMatrices,context->mMatrices,0x40);
    paneCopyBytes(local->mMatrices+0x40,context->mMatrices+0x40,0x30);
    local->mBlend.mType=static_cast<u8>(context->mBlend.mType);local->mBlend.mSrcFactor=static_cast<u8>(context->mBlend.mSrcFactor);local->mBlend.mDstFactor=static_cast<u8>(context->mBlend.mDstFactor);
    local->mLinePart.mType=static_cast<u8>(context->mLinePart.mType);local->mLinePart.mSrcFactor=static_cast<u8>(context->mLinePart.mSrcFactor);local->mLinePart.mDstFactor=static_cast<u8>(context->mLinePart.mDstFactor);
    local->mBoxPart.mType=static_cast<u8>(context->mBoxPart.mType);local->mBoxPart.mSrcFactor=static_cast<u8>(context->mBoxPart.mSrcFactor);local->mBoxPart.mDstFactor=static_cast<u8>(context->mBoxPart.mDstFactor);
    u32 type=gabi::call_ptr<u32>(context->__vtbl->getGrafType,context);
    if(type!=1) clip=0;
    J2DPane* parent=nullptr;
    JSUPtrList* list=mPaneTree.link.mList;
    u8 visible=mVisible;
    if(list) parent=static_cast<J2DPane*>(static_cast<void*>(reinterpret_cast<J2DPaneTree*>(list)->link.mObject));
    if(!visible) return;
    f32 paneRight=mBounds.f.x,paneLeft=mBounds.i.x;
    if(paneRight<paneLeft) return;
    f32 paneBottom=mBounds.f.y,paneTop=mBounds.i.y;
    if(paneBottom<paneTop) return;
    mDrawBounds.i.yBits=static_cast<u32>(mBounds.i.yBits);mDrawBounds.i.xBits=static_cast<u32>(mBounds.i.xBits);
    mGlobalBounds.f.x=paneRight;mGlobalBounds.f.y=paneBottom;
    mDrawBounds.f.xBits=static_cast<u32>(mBounds.f.xBits);mGlobalBounds.i.y=paneTop;
    mDrawBounds.f.yBits=static_cast<u32>(mBounds.f.yBits);mGlobalBounds.i.x=paneLeft;
    if(parent) {
        f32 parentX=parent->mGlobalBounds.i.x,childX=mGlobalBounds.i.x;
        f32 parentY=parent->mGlobalBounds.i.y,childY=mGlobalBounds.i.y;
        f32 childRight=mGlobalBounds.f.x,childBottom=mGlobalBounds.f.y;
        mGlobalBounds.i.x=gabi::fadds_ppc(childX,parentX);mGlobalBounds.i.y=gabi::fadds_ppc(childY,parentY);
        mGlobalBounds.f.x=gabi::fadds_ppc(childRight,parentX);mGlobalBounds.f.y=gabi::fadds_ppc(childBottom,parentY);
        paneConcat(&parent->mDrawMtx,&mMtx,&mDrawMtx);
        if(clip) {
            f32 childLeft=mDrawBounds.i.x,childBottom=mDrawBounds.f.y;
            parentX=parent->mGlobalBounds.i.x;parentY=parent->mGlobalBounds.i.y;
            f32 childTop=mDrawBounds.i.y,childRight=mDrawBounds.f.x;
            childLeft=gabi::fadds_ppc(childLeft,parentX);
            mDrawBounds.i.x=childLeft;mDrawBounds.f.y=gabi::fadds_ppc(childBottom,parentY);
            mDrawBounds.i.y=gabi::fadds_ppc(childTop,parentY);mDrawBounds.f.x=gabi::fadds_ppc(childRight,parentX);
            f32 parentLeft=parent->mDrawBounds.i.x;
            if(!(childLeft>parentLeft)) mDrawBounds.i.x=parentLeft;
            f32 parentTop=parent->mDrawBounds.i.y;childTop=mDrawBounds.i.y;
            if(!(childTop>parentTop)) mDrawBounds.i.y=parentTop;
            f32 parentRight=parent->mDrawBounds.f.x;childRight=mDrawBounds.f.x;
            childBottom=mDrawBounds.f.y;
            if(!(childRight<parentRight)) mDrawBounds.f.x=parentRight;
            f32 parentBottom=parent->mDrawBounds.f.y;
            if(!(childBottom<parentBottom)) mDrawBounds.f.y=parentBottom;
        }
        u8 influenced=mInfluencedAlpha,alpha=mAlpha;
        mDrawAlpha=alpha;
        if(influenced) { u8 parentAlpha=parent->mDrawAlpha;alpha=mAlpha;mDrawAlpha=(alpha*parentAlpha)/255; }
    } else {
        f32 gx=mGlobalBounds.i.x,gb=mGlobalBounds.f.y,gy=mGlobalBounds.i.y,gr=mGlobalBounds.f.x;
        mGlobalBounds.i.x=gabi::fadds_ppc(gx,x);mGlobalBounds.f.y=gabi::fadds_ppc(gb,y);
        mGlobalBounds.i.y=gabi::fadds_ppc(gy,y);mGlobalBounds.f.x=gabi::fadds_ppc(gr,x);
        J2DPaneVtable* table=__vtbl;
        f32 by=mBounds.i.y,bx=mBounds.i.x;
        gabi::call_ptr<void>(table->makeMatrix,this,gabi::fadds_ppc(bx,x),gabi::fadds_ppc(by,y));
        paneCopyMatrix(&mMtx,&mDrawMtx);
        f32 bottom=mGlobalBounds.f.y;u8 alpha=mAlpha;
        mDrawBounds.f.y=bottom;mDrawAlpha=alpha;
        f32 right=mGlobalBounds.f.x,top=mGlobalBounds.i.y,left=mGlobalBounds.i.x;
        mDrawBounds.f.x=right;mDrawBounds.i.x=left;mDrawBounds.i.y=top;
    }
    gabi::FrameLocal<J2DBox> scissor(8);
    f32 zero=sZero;
    scissor->i.x=zero;scissor->f.x=zero;scissor->f.y=zero;scissor->i.y=zero;
    if(clip) paneScissorBounds(context,scissor.get(),&mDrawBounds);
    f32 drawRight=mDrawBounds.f.x,drawLeft=mDrawBounds.i.x;
    bool valid=false;
    if(!(drawRight<drawLeft)) { f32 drawBottom=mDrawBounds.f.y,drawTop=mDrawBounds.i.y;valid=!(drawBottom<drawTop); }
    if(!valid && clip) return;
    panePlaceContext(local.get(),&mGlobalBounds);
    if(clip) { paneScissorContext(local.get(),scissor.get());paneSetScissor(local.get()); }
    gabi::call_ptr<void>(__vtbl->drawSelf,this,&local->mMatrices[0x40],x,y);
    JSUPtrLink* child=mPaneTree.children.mHead;
    while(child) {
        J2DPane* object=static_cast<J2DPane*>(static_cast<void*>(child->mObject));
        object->draw(zero,zero,context,clip);
        child=child->mNext;
    }
}
/* 027EEC60 */
J2DPane* J2DPane::search(u32 tag) {
    if(tag==mTag) return this;
    JSUPtrLink* child=mPaneTree.children.mHead;
    while(child) {
        J2DPane* object=static_cast<J2DPane*>(static_cast<void*>(child->mObject));
        J2DPane* found=gabi::call_ptr<J2DPane*>(object->__vtbl->search,object,tag);
        if(found) return found;
        child=child->mNext;
    }
    return nullptr;
}
/* 027EECF8 */
void J2DPane::makeMatrix(f32 x,f32 y) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    f32 rotation=mRotation,zero=sZero;
    if(rotation!=zero) {
        gabi::FrameLocal<Mtx34> translation(8),rotationMatrix(0x38),origin(0x68);
        f32 baseX=mBasePosition.x,baseY=mBasePosition.y;
        paneTranslate(translation.get(),-baseX,-baseY,zero);
        u32 axis=mRotationAxis;rotation=mRotation;
        if(axis=='z') rotation=-rotation;
        f32 angle=gabi::fmuls_ppc(rotation,sDegreesToRadians);
        paneRotate(rotationMatrix.get(),axis,angle);
        baseX=mBasePosition.x;baseY=mBasePosition.y;
        paneTranslate(origin.get(),gabi::fadds_ppc(baseX,x),gabi::fadds_ppc(baseY,y),zero);
        paneConcat(rotationMatrix.get(),translation.get(),&mMtx);
        paneConcat(origin.get(),&mMtx,&mMtx);
    } else paneTranslate(&mMtx,x,y,zero);
}
/* 027EEE3C */
static void __sinit_J2DPane_cpp() {
    sStaticState.words[2]=0;sStaticState.words[0]=0;sStaticState.words[3]=0;sStaticState.words[1]=0;
    paneRegisterStatic(&sStaticRecord);
}
/* 027EEE64 */
void J2DPane::calcMtx() {
    if(mPaneTree.link.mList) gabi::call_ptr<void>(__vtbl->makeMatrix,this,static_cast<f32>(mBounds.i.x),static_cast<f32>(mBounds.i.y));
}
/* 027EEE88 */
void J2DPane::drawSelf(f32,f32) {}
/* WWHD glue
data sPaneVtable 1016DF1C
data sZero 1016DEF8
data sHalf 1016DEFC
data sDegreesToRadians 1016DF10
data sStaticState 104B4538
data sStaticRecord 101F9844
ext 02842C14 s32 paneRead(JSUInputStream*,void*,s32)
ext 02842D44 s32 paneAlign(JSURandomInputStream*,s32)
ext C000A848 void* paneCopyBytes(void*,const void*,u32)
ext 028E90D4 void paneCopyMatrix(const Mtx34*,Mtx34*)
ext 027EDFC0 void paneScissorBounds(const J2DGrafContext*,J2DBox*,const J2DBox*)
ext 027EDCE8 void panePlaceContext(J2DGrafContext*,const J2DBox*)
ext 027EDD2C void paneScissorContext(J2DGrafContext*,const J2DBox*)
ext 027EDD9C void paneSetScissor(J2DGrafContext*)
ext 0273AD10 void* paneAllocate(u32)
ext 0273AF40 void paneDelete(void*)
ext 027EC678 void paneListInit(JSUPtrList*)
ext 027EC50C void paneLinkConstruct(JSUPtrLink*,void*)
ext 027EC62C void paneLinkDestroy(JSUPtrLink*,u32)
ext 027EC68C void paneListDestroy(JSUPtrList*,u32)
ext 027EC6F4 u32 paneListAppend(JSUPtrList*,JSUPtrLink*)
ext 028E93CC void paneTranslate(Mtx34*,f32,f32,f32)
ext 028E98C0 void paneRotate(Mtx34*,u32,f32)
ext 028E9108 void paneConcat(const Mtx34*,const Mtx34*,Mtx34*)
ext 028F026C void paneRegisterStatic(void*)
*/
