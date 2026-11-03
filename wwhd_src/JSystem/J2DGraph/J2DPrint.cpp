/* Buffer management, following tww J2DPrint. HD: freed previous buffers return null. */
#include "JSystem/J2DGraph/J2DPrint.h"
#include "JSystem/J2DGraph/J2DPane.h"
static J2DPrintBuffer sBuffer;
static J2DPrintStaticState sStaticState;
static u8 sStaticRecord;
// HD: this TU emits a separate copy of the pane's inline matrix-dispatch helper.
/* 027F00E8 */
static void J2DPrintPane_calcMtx(J2DPane* pane) {
    if(pane->mPaneTree.link.mList) gabi::call_ptr<void>(pane->__vtbl->makeMatrix,pane,static_cast<f32>(pane->mBounds.i.x),static_cast<f32>(pane->mBounds.i.y));
}
/* 027F010C */
static J2DColor* J2DPrintColor__ct(J2DColor* color) {
    if(!color) color=static_cast<J2DColor*>(printColorAllocate(sizeof(J2DColor)));
    if(color) color->rgba=0xFFFFFFFF;
    return color;
}
/* 027F0148 */
be<u8>* J2DPrint::setBuffer(u32 size) {
    if(size==0) printBufferAssert("J2DPrint.cpp",325,"size > 0");
    u8 owned=sBuffer.mHeapFlag;
    be<u8>* previous=sBuffer.mText;
    if(owned) { printBufferDelete(previous);previous=nullptr; }
    void* heap=printBufferSystemHeap();
    be<u8>* text=static_cast<be<u8>*>(printBufferAllocate(size,heap,4));
    sBuffer.mText=text; sBuffer.mSize=size;sBuffer.mHeapFlag=1;
    return previous;
}
/* 027F01F4 */
static void __sinit_J2DPrint_cpp() {
    sStaticState.words[2]=0;sStaticState.words[0]=0;sStaticState.words[3]=0;sStaticState.words[1]=0;
    printBufferRegisterStatic(&sStaticRecord);
}
/* WWHD glue
data sBuffer 101F9868
data sStaticState 104B4558
data sStaticRecord 101F985C
str 1016E028 "J2DPrint.cpp"
str 1016E038 "size > 0"
ext 0273AD10 void* printColorAllocate(u32)
ext 0273AF40 void printBufferDelete(void*)
ext 027EC42C void* printBufferSystemHeap()
ext 0273B0D4 void* printBufferAllocate(u32,void*,s32)
ext 0273AA24 void printBufferAssert(const char*,u32,const char*)
ext 028F026C void printBufferRegisterStatic(void*)
*/
