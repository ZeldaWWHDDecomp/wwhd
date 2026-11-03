/** JPA1 field archive constructor; getters belong to the merged JPAFieldBlock TU. */
#include "bindings.h"
#include "JSystem/JParticle/JPAFieldBlock_archive.h"
static u32 sVtable;
static JPAFieldArchiveStaticBlock sStaticBlock;
static u32 sStaticRegister;
/* 028482B8 */
static JPAFieldBlockArc* JPAFieldBlockArc__ct(JPAFieldBlockArc* object,const u8* data){
 if(!object) object=(JPAFieldBlockArc*)operator_new(sizeof(JPAFieldBlockArc));
 if(object){object->mpData=(const JPAFieldBlockData*)(data+0xC);object->mVtable=&sVtable;}
 return object;
}
/* 0284830C */
static void __sinit_JPAFieldBlock_cpp(){
 sStaticBlock.words[2]=0;sStaticBlock.words[0]=0;sStaticBlock.words[3]=0;sStaticBlock.words[1]=0;
 JPAFieldArchiveRegister(&sStaticRegister);
}
/* WWHD glue
 data sVtable 1017AEE8
 data sStaticBlock 104B66B4
 data sStaticRegister 101FC744
 ext 028F026C void JPAFieldArchiveRegister(void*)
*/
