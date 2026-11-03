/** JKRDisposer platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRDisposer.h"
static JKRDisposerStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EBD44 */
static void __sinit_JKRDisposer_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRDisposerConstructRegister(&sStaticRegister);
}


/* 027EBD2C */
static void JKRDisposerBase__dt(void* object,u32 flags) {
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 027EBD40 */
static void JKRDisposerBase_reserved() {}

/* WWHD glue

 data sStaticBlock 104A2410
 data sStaticRegister 101F9760
 ext 028F026C void JKRDisposerConstructRegister(void*)
*/
