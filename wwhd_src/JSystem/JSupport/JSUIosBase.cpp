/** JSUIosBase platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JSupport/JSUIosBase.h"
static JSUIosBaseStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02842BB4 */
static void __sinit_JSUIosBase_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JSUIosBaseConstructRegister(&sStaticRegister);
}

/* WWHD glue
 data sStaticBlock 104B64E4
 data sStaticRegister 101FAF9C
 ext 028F026C void JSUIosBaseConstructRegister(void*)
*/
