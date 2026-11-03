// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGActor.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 02842304 */
s32 JStage::TActor::JSGFGetType() const {
    return 2;
}

/* 0284230C */
f32 JStage::TActor::JSGGetAnimationFrame() const {
    return 0.0f;
}

/* 02842318 */
f32 JStage::TActor::JSGGetAnimationFrameMax() const {
    return 0.0f;
}

/* 02842324 */
f32 JStage::TActor::JSGGetAnimationTransition() const {
    return 0.0f;
}

/* 02842330 */
void JStage::TActor::destroy(s32 flags) {
    if (!this) return;
    TObject::destroy(0);
    if (flags & 1) JStage_delete(this);
}

namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}

/* 028423EC */
static void __sinit_JSGActor_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sStaticBlock 104B6444
data sStaticRegister 101FAEE8
*/
