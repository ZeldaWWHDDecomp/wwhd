/* Additional JUTDbPrint methods, following tww. HD: flush discards entries without GX drawing. */
#include "JSystem/JUtility/JUTDbPrint_extra.h"
struct PrintSlot { gptr<JUTDbPrint> print; };
static PrintSlot sDebugPrint;
static JUTDbPrintStatic sStaticState;
static u8 sStaticRecord;
/* 027ECC0C */
JUTDbPrint* JUTDbPrint::start(JUTFont* font,JKRHeap* heap) {
    JUTDbPrint* print=sDebugPrint.print;
    if(!print) {
        if(!heap) heap=JUTDbPrintCurrentHeap();
        print=printConstruct(nullptr,font,heap);
        sDebugPrint.print=print;
    }
    return print;
}
/* 027ECC70 */
void JUTDbPrint::flush() {
    JUTDbPrint* print=this;
    JUTPrintEntry* entry=static_cast<JUTPrintEntry*>(static_cast<void*>(print->mFirst));
    while(entry) {
        JUTPrintEntry* next=entry->mNext;
        printFree(print->mHeap,entry);
        entry=next;
    }
    print->mFirst=nullptr;
}
/* 027ECCC8 */
JUTFont* JUTDbPrint::changeFont(JUTFont* font) {
    JUTDbPrint* print=this;
    JUTFont* previous=print->mFont;
    if(font) print->mFont=font;
    return previous;
}
/* 027ECCE0 */
void JUTDbPrint::destroyEntries() { flush(); }
/* 027ECCE4 */
static void __sinit_JUTDbPrint_extra_cpp() {
    sStaticState.words[2]=0;sStaticState.words[0]=0;sStaticState.words[3]=0;sStaticState.words[1]=0;
    printRegisterStatic(&sStaticRecord);
}
/* WWHD glue
data sDebugPrint 101F97D4
data sStaticState 104A2490
data sStaticRecord 101F97C8
ext 027EC230 JKRHeap* JUTDbPrintCurrentHeap()
ext 027ECB40 JUTDbPrint* printConstruct(JUTDbPrint*,JUTFont*,JKRHeap*)
ext 027EC104 void printFree(JKRHeap*,void*)
ext 028F026C void printRegisterStatic(void*)
*/
