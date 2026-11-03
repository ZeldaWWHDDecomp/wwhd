// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGCamera.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 02842498 */
s32 JStage::TCamera::JSGFGetType() const {
    return 3;
}

/* 028424A0 */
void JStage::TCamera::destroy(s32 flags) {
    if (!this) return;
    TObject::destroy(0);
    if (flags & 1) JStage_delete(this);
}

namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}

/* 0284255C */
static void __sinit_JSGCamera_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sStaticBlock 104B6464
data sStaticRegister 101FAF00
*/
