/** vector platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JGadget/vector.h"
static vectorStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 028434A8 */
static void __sinit_vector_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    vectorConstructRegister(&sStaticRegister);
}


/* 028434D0 */
static JGadget::TVector_pointer* TVector_pointer__ct(JGadget::TVector_pointer* vector) {
    if (vector == NULL) vector = (JGadget::TVector_pointer*)operator_new(sizeof(JGadget::TVector_pointer));
    if (vector != NULL) {
        vector->mCapacity = 0;
        vector->mExtend = gabi::guest_fn((const void*)JGadgetVectorExtendDefault);
        vector->mEnd = NULL;
        vector->mBegin = NULL;
    }
    return vector;
}
/* 02843544 */
static void TVector_pointer__dt(JGadget::TVector_pointer* vector,u32 flags) {
    if (vector == NULL) return;
    be<u32>* begin = vector->mBegin;
    // Pointer elements have trivial destruction; GHS's traversal has no visible effects.
    vector->mEnd = begin;
    operator_delete(vector->mBegin);
    if (flags & 1) operator_delete(vector);
}
// HD: inlined element copies use overlap-safe byte moves, retaining the binary's copy direction.
static void moveVectorBytes(be<u8>* destination,const be<u8>* source,u32 bytes) {
    if (bytes == 0) return;
    u32 delta = gabi::ea(destination) - gabi::ea(source);
    if ((u64)(s64)(s32)delta <= (u64)(s64)(s32)bytes) {
        for (u32 i = bytes; i != 0;) { --i; destination[i] = source[i]; }
    } else {
        for (u32 i = 0; i < bytes; ++i) destination[i] = source[i];
    }
}
/* 0284364C */
be<u32>* JGadget::TVector_pointer::insert(be<u32>* position,const be<u32>* value) {
    be<u32>* begin = mBegin;
    u32 capacity = mCapacity;
    s32 originalIndex = (s32)(gabi::ea(position) - gabi::ea(begin)) >> 2;
    u32 size = begin != NULL ? (u32)((s32)(gabi::ea((be<u32>*)mEnd) - gabi::ea(begin)) >> 2) : 0;
    be<u32>* slot = position;
    if (size + 1 <= capacity) {
        be<u32>* end = mEnd;
        if (gabi::ea(position + 1) < gabi::ea(end)) {
            be<u32>* last = end - 1;
            moveVectorBytes((be<u8>*)end,(const be<u8>*)last,4);
            end = mEnd;
            u32 bytes = (u32)((s32)(gabi::ea(last) - gabi::ea(position)) >> 2) * 4;
            be<u8>* destination = (be<u8>*)end - bytes;
            moveVectorBytes(destination,(const be<u8>*)position,bytes);
            end = mEnd;
        } else {
            u32 bytes = (u32)((s32)(gabi::ea(end) - gabi::ea(position)) >> 2) * 4;
            moveVectorBytes((be<u8>*)(position + 1),(const be<u8>*)position,bytes);
            end = mEnd;
        }
        mEnd = end + 1;
    } else {
        u32 extension = mExtend;
        u32 currentSize = begin != NULL ? (u32)((s32)(gabi::ea((be<u32>*)mEnd) - gabi::ea(begin)) >> 2) : 0;
        u32 required = currentSize + 1;
        u32 extended = gabi::call_ptr<u32>(extension,capacity,currentSize,1);
        u32 newCapacity = extended > required ? extended : required;
        be<u32>* newBegin = (be<u32>*)operator_new(newCapacity * 4);
        if (newBegin == NULL) return (be<u32>*)mBegin + originalIndex;
        be<u32>* oldBegin = mBegin;
        u32 prefixBytes = (u32)((s32)(gabi::ea(position) - gabi::ea(oldBegin)) >> 2) * 4;
        moveVectorBytes((be<u8>*)newBegin,(const be<u8>*)oldBegin,prefixBytes);
        be<u32>* oldEnd = mEnd;
        u32 suffixBytes = (u32)((s32)(gabi::ea(oldEnd) - gabi::ea(position)) >> 2) * 4;
        slot = (be<u32>*)((be<u8>*)newBegin + prefixBytes);
        moveVectorBytes((be<u8>*)(slot + 1),(const be<u8>*)position,suffixBytes);
        oldBegin = mBegin;
        oldEnd = mEnd;
        u32 oldBytes = (u32)((s32)(gabi::ea(oldEnd) - gabi::ea(oldBegin)) >> 2) * 4;
        mCapacity = newCapacity;
        mBegin = newBegin;
        mEnd = (be<u32>*)((be<u8>*)newBegin + oldBytes + 4);
        operator_delete(oldBegin);
    }
    if (slot != (be<u32>*)mEnd) *slot = *value;
    return (be<u32>*)mBegin + originalIndex;
}

/* WWHD glue
 ext 02843AEC u32 JGadgetVectorExtendDefault(u32,u32,u32) callback
 data sStaticBlock 104B6564
 data sStaticRegister 101FAFFC
 ext 028F026C void vectorConstructRegister(void*)
*/
