/** JKRHeap_current platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRHeap_current.h"
static JKRHeap_currentStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EC438 */
static void __sinit_JKRHeap_current_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRHeap_currentConstructRegister(&sStaticRegister);
}


static JKRHeap_l* sCurrentSystemHeap;
/* 027EC42C */
JKRHeap_l* JKRHeap_l::getSystemHeap() { return sCurrentSystemHeap; }

/* WWHD glue
 data sCurrentSystemHeap 104A244C
 data sStaticBlock 104A2450
 data sStaticRegister 101F9798
 ext 028F026C void JKRHeap_currentConstructRegister(void*)
*/
