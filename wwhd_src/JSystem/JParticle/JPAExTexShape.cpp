/** JPA1 extra-texture archive accessors. */
#include "bindings.h"
#include "JSystem/JParticle/JPAExTexShape.h"
static u32 sVtable;
static JPAExTexShapeStaticBlock sStaticBlock;
static u32 sStaticRegister;
/* 02847EB8 */
u32 JPAExTexShapeArc::getIndTexMode(){return (u32)mpData->mFlags&3;}
/* 02847EC8 */
const be<f32>* JPAExTexShapeArc::getIndTexMtx(){return &mpData->mIndTexMtx[0][0];}
/* 02847ED4 */
s32 JPAExTexShapeArc::getExpScale(){return mpData->mExpScale;}
/* 02847EE4 */
u8 JPAExTexShapeArc::getIndTextureID(){return mpData->mIndTextureID;}
/* 02847EF0 */
u8 JPAExTexShapeArc::getSubTextureID(){return mpData->mSubTextureID;}
/* 02847EFC */
u32 JPAExTexShapeArc::isEnableSecondTex(){return (u32)mpData->mFlags&0x100;}
/* 02847F0C */
u8 JPAExTexShapeArc::getSecondTexIndex(){return mpData->mSecondTextureID;}
/* 02847E3C */
static JPAExTexShapeArc* JPAExTexShapeArc__ct(JPAExTexShapeArc* object,const u8* data){
 if(!object) object=(JPAExTexShapeArc*)operator_new(sizeof(JPAExTexShapeArc));
 if(object){object->mpData=(const JPAExTexShapeData*)(data+0xC);object->mVtable=&sVtable;}
 return object;
}
/* 02847E90 */
static void __sinit_JPAExTexShape_cpp(){
 sStaticBlock.words[2]=0;sStaticBlock.words[0]=0;sStaticBlock.words[3]=0;sStaticBlock.words[1]=0;
 JPAExTexShapeRegister(&sStaticRegister);
}
/* WWHD glue
 data sVtable 1017AD30
 data sStaticBlock 104B6694
 data sStaticRegister 101FC72C
 ext 028F026C void JPAExTexShapeRegister(void*)
*/
