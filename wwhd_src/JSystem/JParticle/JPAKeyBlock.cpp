/** GameCube JPA1 JPAKeyBlock archive constructor and platform registration. */
#include "bindings.h"
#include "JSystem/JParticle/JPAKeyBlock.h"
static u32 sVtable;
static JPAKeyBlockStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02848430 */
static JPAKeyBlockArc* JPAKeyBlockArc__ct(JPAKeyBlockArc* object,const u8* data) {
    if (object == NULL) object = (JPAKeyBlockArc*)operator_new(sizeof(JPAKeyBlockArc));
    if (object != NULL) {
        object->mpData = (const JPAKeyBlockData*)(data + 0x0c);
        object->mVtable = &sVtable;
        object->mpKeyData = (const be<f32>*)(data + 0x20);
    }
    return object;
}

/* 0284848C */
static void __sinit_JPAKeyBlock_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JPAKeyBlockConstructRegister(&sStaticRegister);
}

/* WWHD glue
 data sVtable 1017AF88
 data sStaticBlock 104B66C4
 data sStaticRegister 101FC750
 ext 028F026C void JPAKeyBlockConstructRegister(void*)
*/
