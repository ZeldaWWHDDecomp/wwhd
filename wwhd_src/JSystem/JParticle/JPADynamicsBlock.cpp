/** GameCube JPA1 JPADynamicsBlock archive constructor and platform registration. */
#include "bindings.h"
#include "JSystem/JParticle/JPADynamicsBlock.h"
static u32 sVtable;
static JPADynamicsBlockStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 0284B45C */
static JPADynamicsBlockArc* JPADynamicsBlockArc__ct(JPADynamicsBlockArc* object,const u8* data) {
    if (object == NULL) object = (JPADynamicsBlockArc*)operator_new(sizeof(JPADynamicsBlockArc));
    if (object != NULL) {
        object->mpData = (const JPADynamicsBlockData*)(data + 0x0c);
        object->mVtable = &sVtable;
    }
    return object;
}

/* 0284B4B0 */
static void __sinit_JPADynamicsBlock_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JPADynamicsBlockConstructRegister(&sStaticRegister);
}

/* WWHD glue
 data sVtable 1017B5E0
 data sStaticBlock 104B6734
 data sStaticRegister 101FC80C
 ext 028F026C void JPADynamicsBlockConstructRegister(void*)
*/
