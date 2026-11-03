// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGSystem.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 028427B0 */
s32 JStage::TSystem::JSGFGetType() const {
    return 1;
}

/* 02842734 */
void JStage::TSystem::destroy(s32 flags) {
    if (!this) return;
    TObject::destroy(0);
    if (flags & 1) JStage_delete(this);
}

namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}

/* 028422DC */
static void __sinit_JSGSystem_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sStaticBlock 104B6434
data sStaticRegister 101FAEDC
*/
