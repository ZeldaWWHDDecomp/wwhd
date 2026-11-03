/* JGadget pointer-vector growth companion; HD doubles the current element count. */
#include "bindings.h"
#include "JSystem/JGadget/vector_extra.h"
static vectorStaticBlock vectorHeaderStatics;
static VectorDestructorRecord vectorDestructorRecord;
/* 02843AEC */
u32 JGadgetVectorExtendDefault(u32 capacity,u32 size,u32 count) {
    return size * 2;
}
/* 02843AF4 */
void __sinit_vector_extra_cpp() {
    vectorHeaderStatics.mWords[2] = 0;
    vectorHeaderStatics.mWords[0] = 0;
    vectorHeaderStatics.mWords[3] = 0;
    vectorHeaderStatics.mWords[1] = 0;
    registerVectorCompanion(&vectorDestructorRecord);
}
/* WWHD glue
data vectorHeaderStatics 104B6574
data vectorDestructorRecord 101FB008
ext 028F026C void registerVectorCompanion(VectorDestructorRecord*)
*/
