/* Screen header parsing, following tww J2DScreen. HD: GX background drawing is omitted. */
#include "JSystem/J2DGraph/J2DScreen.h"
static f32 sZero=0.0f;
static f32 sWidth=640.0f;
static f32 sHeight=480.0f;
static f32 sNear=-1.0f;
static f32 sFar=1.0f;

struct ScreenStaticState { be<u32> words[4]; };
static ScreenStaticState sStaticState;
static u8 sStaticRecord;
static JSUStreamVtable sMemoryVtable;
static ScreenStaticState sMainStaticState;
static u8 sMainStaticRecord;
using ScreenPaneHeader=J2DPane::J2DScrnBlockHeader;
struct ScreenVtable : J2DPaneVtable { u8 pad68[4];be<u32> createPane; };

/* 027F021C */
static void J2DScreen__dt(J2DScreen* screen,u32 flags) {
    if(screen) { screenPaneDestroy(screen,0);if(flags&1) screenDelete(screen); }
}
/* 027F0270 */
bool J2DScreen::checkSignature(JSURandomInputStream* stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<J2DScreenHeader> header(8);
    screenRead(stream,header.get(),sizeof(J2DScreenHeader));
    return header->magic==0x5343524E && header->type==0x626C6F31;
}
/* 027F02DC */
bool J2DScreen::getScreenInformation(JSURandomInputStream* stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    gabi::FrameLocal<J2DScreenInfo> info(0x14);
    screenRead(stream,info.get(),sizeof(J2DScreenInfo));
    if(info->magic!=0x494E4631) return false;
    f32 width=static_cast<f32>(static_cast<u16>(info->width));
    f32 height=static_cast<f32>(static_cast<u16>(info->height));
    J2DPaneVtable* vtable=__vtbl;
    f32 zero=sZero;
    mBounds.i.x=zero;mBounds.i.y=zero;mBounds.f.x=width;mBounds.f.y=height;
    gabi::call_ptr<void>(vtable->calcMtx,this);
    mColor=static_cast<u32>(info->color);
    u32 size=info->size;
    if(size>sizeof(J2DScreenInfo)) gabi::call_ptr<s32>(stream->mVtable->mSkip[1],stream,size-sizeof(J2DScreenInfo));
    return true;
}
/* 027F03F4 */
s32 J2DScreen::makeHierarchyPanes(J2DPane* parent,JSURandomInputStream* stream) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    J2DPane* pane=parent;
    gabi::FrameLocal<ScreenPaneHeader> header(8);
    for(;;) {
        screenPeek(stream,header.get(),8);
        u32 magic=header->magic;
        if(magic==0x42474E31) {
            screenSeek(stream,header->size,1);
            s32 result=makeHierarchyPanes(pane,stream);
            if(result) return result;
        } else if(magic==0x454E4431) {
            screenSeek(stream,header->size,1);return 0;
        } else if(magic==0x45585431) {
            screenSeek(stream,header->size,1);return 1;
        } else {
            ScreenVtable* table=static_cast<ScreenVtable*>(__vtbl.get());
            pane=gabi::call_ptr<J2DPane*>(table->createPane,this,header.get(),stream,parent);
            if(!pane) return 2;
        }
    }
}
/* 027F0528 */
bool J2DScreen::set(JSURandomInputStream* stream) {
    if(!checkSignature(stream)) return false;
    if(!getScreenInformation(stream)) return false;
    if(makeHierarchyPanes(this,stream)==2) return false;
    return stream->mState==0;
}
/* 027F05BC */
bool J2DScreen::set(const char* name,JKRArchive* archive) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    const void* resource=screenFindResource(name,archive);
    if(!resource) return false;
    u32 size=archive->getResSize(resource);
    gabi::FrameLocal<JSUMemoryInputStream> stream(8);
    stream->mState=0;stream->mVtable=&sMemoryVtable;
    screenMemorySet(stream.get(),resource,size);
    bool result=set(stream.get());
    screenMemoryDestroy(stream.get(),0);
    return result;
}
static void copyScreenBox(J2DBox* output,const J2DBox* input) {
    output->i.xBits=static_cast<u32>(input->i.xBits);output->i.yBits=static_cast<u32>(input->i.yBits);
    output->f.xBits=static_cast<u32>(input->f.xBits);output->f.yBits=static_cast<u32>(input->f.yBits);
}
static void copyScreenBlend(J2DBlend* output,const J2DBlend* input) {
    output->mType=static_cast<u8>(input->mType);output->mSrcFactor=static_cast<u8>(input->mSrcFactor);output->mDstFactor=static_cast<u8>(input->mDstFactor);
}
/* 027F0688 */
void J2DScreen::draw(f32 x,f32 y,const J2DGrafContext* context) {
    gabi::AutoFrame nativeFrame{gabi::current_function()};
    if(context) {
        gabi::FrameLocal<J2DGrafContext> local(8);
        copyScreenBox(&local->mBounds,&context->mBounds);copyScreenBox(&local->mScissorBounds,&context->mScissorBounds);
        local->__vtbl=J2DGetBaseVtable();
        local->mColorTL.rgba=static_cast<u32>(context->mColorTL.rgba);local->mColorTR.rgba=static_cast<u32>(context->mColorTR.rgba);
        local->mColorBR.rgba=static_cast<u32>(context->mColorBR.rgba);local->mColorBL.rgba=static_cast<u32>(context->mColorBL.rgba);
        local->mLineWidth=static_cast<u8>(context->mLineWidth);
        local->mPrevPos.xBits=static_cast<u32>(context->mPrevPos.xBits);local->mPrevPos.yBits=static_cast<u32>(context->mPrevPos.yBits);
        screenCopyBytes(local->mMatrices,context->mMatrices,0x40);screenCopyBytes(local->mMatrices+0x40,context->mMatrices+0x40,0x30);
        copyScreenBlend(&local->mBlend,&context->mBlend);copyScreenBlend(&local->mLinePart,&context->mLinePart);copyScreenBlend(&local->mBoxPart,&context->mBoxPart);
        screenPaneDraw(this,context,mbClipToParent,x,y);
        screenSetScissor(local.get());
    } else {
        gabi::FrameLocal<J2DOrthoGraph> local(8);
        f32 nearPlane=sNear,zero=sZero,height=sHeight,width=sWidth,farPlane=sFar;
        screenOrthoConstruct(local.get(),zero,zero,width,height,nearPlane,farPlane);
        screenOrthoSetPort(local.get());
        screenPaneDraw(this,reinterpret_cast<J2DGrafContext*>(local.get()),mbClipToParent,x,y);
        screenSetScissor(reinterpret_cast<J2DGrafContext*>(local.get()));
    }
}
/* 027F08D4 */
static void __sinit_J2DScreen_cpp() {
    sMainStaticState.words[2]=0;sMainStaticState.words[0]=0;sMainStaticState.words[3]=0;sMainStaticState.words[1]=0;
    screenRegisterStatic(&sMainStaticRecord);
}
/* 027F0868 */
J2DPane* J2DScreen::search(u32 tag) {
    if(!tag) return nullptr;
    return screenPaneSearch(this,tag);
}
/* 027F0898 */
J2DPane* J2DScreen::createPane(const J2DScrnBlockHeader&,JSURandomInputStream*,J2DPane*) {
    screenAssert("J2DScreen.cpp",168,"0");
    return nullptr;
}
/* 027F08D0 */
void J2DScreen::drawSelf(f32,f32,const Mtx34*) {}
/* 027F08FC */
void J2DScreen::calcMtx() {
    gabi::call_ptr<void>(__vtbl->makeMatrix,this,static_cast<f32>(mBounds.i.x),static_cast<f32>(mBounds.i.y));
}
/* 027F0914 */
static void __sinit_J2DScreen_tail_cpp() {
    sStaticState.words[2]=0;sStaticState.words[0]=0;sStaticState.words[3]=0;sStaticState.words[1]=0;
    screenRegisterStatic(&sStaticRecord);
}
/* WWHD glue
data sMemoryVtable 10173B4C
data sZero 1016E058
data sWidth 1016E05C
data sHeight 1016E060
data sNear 1016E064
data sFar 1016E068
data sMainStaticState 104B4568
data sMainStaticRecord 101F9874
data sStaticState 104B4578
data sStaticRecord 101F9880
str 1016E070 "J2DScreen.cpp"
str 1016E06C "0"
ext 02842C6C s32 screenPeek(JSURandomInputStream*,void*,s32)
ext 02842D00 s32 screenSeek(JSURandomInputStream*,s32,u32)
ext C000A848 void* screenCopyBytes(void*,const void*,u32)
ext 027EE7BC void screenPaneDraw(J2DPane*,const J2DGrafContext*,u32,f32,f32)
ext 027EDD9C void screenSetScissor(J2DGrafContext*)
ext 027EDE38 J2DOrthoGraph* screenOrthoConstruct(J2DOrthoGraph*,f32,f32,f32,f32,f32,f32)
ext 027EDF68 void screenOrthoSetPort(J2DOrthoGraph*)
ext 027EBF60 const void* screenFindResource(const char*,JKRArchive*)
ext 02842E54 void screenMemorySet(JSUMemoryInputStream*,const void*,s32)
ext 02842BF4 void screenMemoryDestroy(JSUMemoryInputStream*,u32)
ext 02842C14 s32 screenRead(JSUInputStream*,void*,s32)
ext 027EE6F0 void screenPaneDestroy(J2DPane*,u32)
ext 0273AF40 void screenDelete(void*)
ext 027EEC60 J2DPane* screenPaneSearch(J2DPane*,u32)
ext 0273AA24 void screenAssert(const char*,u32,const char*)
ext 028F026C void screenRegisterStatic(void*)
*/
