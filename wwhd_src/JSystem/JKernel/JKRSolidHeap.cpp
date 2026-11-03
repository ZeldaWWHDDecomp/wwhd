/** JKRSolidHeap_l platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRSolidHeap.h"
static JKRSolidHeapStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EC4CC */
static void __sinit_JKRSolidHeap_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRSolidHeapConstructRegister(&sStaticRegister);
}


static u32 sSafeStringVtable;
/* 027EC460 */
void* JKRSolidHeap_l::adjustSize() { return JKRSolidHeapAdjustAdapter(this); }
/* 027EC464 */
JKRSolidHeap_l* JKRSolidHeap_l::create(u32 size,void* parent) {
    if (size == 0xffffffff) size = 0;
    gabi::Local<JKRSolidHeapName> name;
    name->mVtable = &sSafeStringVtable;
    name->mName.v = gabi::guest_str("JKRSolidHeap");
    JKRSolidHeap_l* heap = JKRSolidHeapCreateAdapter(size,name.get(),parent,1,0);
    if (heap != NULL) heap->mFlags = (u32)heap->mFlags & ~4u;
    return heap;
}

/* WWHD glue
 data sSafeStringVtable 1016DCB0
 ext 02754B38 JKRSolidHeap_l* JKRSolidHeapCreateAdapter(u32,JKRSolidHeapName*,void*,u32,u32)
 ext 02754E74 void* JKRSolidHeapAdjustAdapter(JKRSolidHeap_l*)
 str 1016DCC8 "JKRSolidHeap"
 data sStaticBlock 104A2460
 data sStaticRegister 101F97A4
 ext 028F026C void JKRSolidHeapConstructRegister(void*)
*/
