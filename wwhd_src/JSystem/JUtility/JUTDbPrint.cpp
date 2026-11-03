/** Debug-print setup and reporting, from the GC JUTDbPrint translation unit. */
#include "bindings.h"
#include "JSystem/JUtility/JUTDbPrint.h"
/* 027ECB40 */
static JUTDbPrint* JUTDbPrint__ct(JUTDbPrint* print,JUTFont* font,JKRHeap* heap) {
    if (print == NULL) print = (JUTDbPrint*)operator_new(sizeof(JUTDbPrint));
    if (print != NULL) {
        print->mFont = font;
        print->mColor = 0xffffffff;
        print->mFirst = NULL;
        if (heap == NULL) heap = JUTDbPrintCurrentHeap();
        print->mHeap = heap;
        print->mColor = 0xffffffff;
        print->mVisible = 1;
    }
    return print;
}


// HD: explicit GHS variadic save areas retain integer and optional floating-register arguments.
static void saveJUTReportArguments(OSReportFrame* frame,const u32* registers,bool floats) {
    for (u32 i = 0; i < 8; ++i) frame->gpr[i] = registers[i];
    if (floats) {
        for (u32 i = 0; i < 8; ++i) {
            u64 bits;
            memcpy(&bits,&gabi::cpu->f[i+1].ps0,8);
            frame->fpr[i].high = bits >> 32;
            frame->fpr[i].low = bits;
        }
    }
}
static JUTReportStringVtable sReportStringVtable;
static u32 sReportBufferedStringVtable;
/* 027EC9E8 */
static void JUTReport(u32 x,u32 y,const void* format,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10) {
    u32 entry = gabi::cpu->r[1];
    bool floats = gabi::cpu->cr[6] != 0;
    gabi::Local<OSReportFrame> frame;
    const u32 registers[8] = {x,y,gabi::ea(format),a6,a7,a8,a9,a10};
    saveJUTReportArguments(frame.get(),registers,floats);
    frame->args.overflow_arg_area = entry + 8;
    frame->args.reg_save_area = gabi::ea(frame->gpr);
    frame->args.gpr = 3;
    frame->args.fpr = 0;
    gabi::Local<JUTReportText> buffer;
    if (JUTReportVsnprintf(buffer.get(),256,format,&frame->args) < 0) return;
    // HD: format through bounded SafeString storage; GC debug-screen drawing is absent.
    gabi::Local<JUTReportString> source;
    gabi::Local<JUTReportBufferedString> destination;
    source->mText = buffer->mBytes;
    source->mVtable = &sReportStringVtable;
    destination->mText = destination->mBytes;
    destination->mVtable = &sReportBufferedStringVtable;
    destination->mCapacity = 0x104;
    destination->mBytes[0x103] = 0;
    JUTReportAssureTermination(source.get());
    const be<u8>* text = source->mText;
    s32 length = 0;
    while ((u8)*text != 0) {
        ++length;
        ++text;
        if (length > 0x40000) { length = 0; break; }
    }
    s32 capacity = destination->mCapacity;
    u32 assure = source->mVtable->mAssureTermination;
    if (length >= capacity) length = capacity - 1;
    gabi::call_ptr<void>(assure,source.get());
    JUTReportBlockMove(destination->mBytes,source->mText,length,0);
}

/* WWHD glue
 data sReportStringVtable 1016DCF0
 data sReportBufferedStringVtable 1016DD20
 ext 027EC230 JKRHeap* JUTDbPrintCurrentHeap()
 ext 028F1748 s32 JUTReportVsnprintf(void*,u32,const void*,OSReportVaList*)
 ext 027ECD4C void JUTReportAssureTermination(JUTReportString*)
 ext C0009988 void* JUTReportBlockMove(void*,const void*,u32,u32)
*/
