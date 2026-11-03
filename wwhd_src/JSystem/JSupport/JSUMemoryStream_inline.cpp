/** JSUMemoryStream_inline platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JSupport/JSUMemoryStream_inline.h"
static JSUMemoryStream_inlineStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02842F94 */
static void __sinit_JSUMemoryStream_inline_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JSUMemoryStream_inlineConstructRegister(&sStaticRegister);
}


/* 02842F8C */
static s32 JSUMemoryInputStream_getPosition(const JSUMemoryInputStream* stream) {
    return stream->mPosition;
}

/* WWHD glue

 data sStaticBlock 104B6514
 data sStaticRegister 101FAFC0
 ext 028F026C void JSUMemoryStream_inlineConstructRegister(void*)
*/
