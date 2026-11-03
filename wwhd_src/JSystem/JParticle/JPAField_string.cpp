// HD: compiler-emitted buffered-string lifetime and termination helpers.
#include "JSystem/JParticle/JPAField_string.h"
void JPAFieldStringDelete(void*);

/* 02822870 */
void JPAFieldBufferedString::assureTermination() {
    mBuffer.get()[u32(mBufferSize) - 1] = 0;
}
/* 02822888 */
void JPAFieldDeleteBufferedString(void* self, u32 deleting) {
    if (self && (deleting & 1)) JPAFieldStringDelete(self);
}
/* 0282289C */
void JPAFieldSafeStringAssureTermination(void*) {}
/* 028228A0 */
void JPAFieldDeleteFixedString(void* self, u32 deleting) {
    if (self && (deleting & 1)) JPAFieldStringDelete(self);
}
/* 028228B4 */
void JPAFieldDeleteMutableString(void* self, u32 deleting) {
    if (self && (deleting & 1)) JPAFieldStringDelete(self);
}

/* WWHD glue
ext 0273AF40 void JPAFieldStringDelete(void*)
*/
