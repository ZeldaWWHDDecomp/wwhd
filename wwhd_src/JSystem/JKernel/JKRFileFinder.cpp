/** JKRFileFinder platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRFileFinder.h"
static JKRFileFinderStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EBD04 */
static void __sinit_JKRFileFinder_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRFileFinderConstructRegister(&sStaticRegister);
}


/* 027EBC5C */
static void JKRFileFinderList__dt(JSUPtrList* list,u32 flags) {
    if (list == NULL) return;
    JKRFileFinderListDestroy(list,0);
    if (flags & 1) operator_delete(list);
}

/* WWHD glue
 ext 027EC68C void JKRFileFinderListDestroy(JSUPtrList*,u32)
 data sStaticBlock 104A2400
 data sStaticRegister 101F9754
 ext 028F026C void JKRFileFinderConstructRegister(void*)
*/
