// JStage defaults reconstructed from Cafe, using the GameCube interface names.
#include "JSystem/JStage/JSGObject.h"
void JStage_delete(void*);
void JStage_register(void*);

/* 02842698 */
void JStage::TObject::destroy(s32 flags) {
    if (this && (flags & 1)) JStage_delete(this);
}

/* 0284271C */
void JStage::TObject::JSGSetData(u32, const void*, u32) {

}

/* 02842720 */
void JStage::TObject::JSGGetParent(gptr<TObject>*, be<u32>*) const {

}

/* 02842724 */
void JStage::TObject::JSGSetParent(TObject*, u32) {

}

/* 02842728 */
void JStage::TObject::JSGSetRelation(u32, TObject*, u32) {

}

/* 02842714 */
u32 JStage::TObject::JSGGetFlag() const {
    return 0;
}

/* 0284272C */
s32 JStage::TObject::JSGFindNodeID(const char*) const {
    return -1;
}

/* 028426AC */
bool JStage::TObject::JSGGetNodeTransformation(u32, Mtx34* matrix) const {
    for (u32 row=0; row<3; ++row)
        for (u32 column=0; column<4; ++column) matrix->m[row][column]=0.0f;
    return false;
}

namespace {
struct StaticBlock { be<u32> words[4]; };
static StaticBlock sStaticBlock;
static u32 sStaticRegister;
}

/* 02842788 */
static void __sinit_JSGObject_cpp() {
    sStaticBlock.words[2]=0;
    sStaticBlock.words[0]=0;
    sStaticBlock.words[3]=0;
    sStaticBlock.words[1]=0;
    JStage_register(&sStaticRegister);
}



/* WWHD glue:
data sStaticBlock 104B6494
data sStaticRegister 101FAF24
ext 0273AF40 void JStage_delete(void*)
ext 028F026C void JStage_register(void*)
*/
