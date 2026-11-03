/** binary platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JGadget/binary.h"
static binaryStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 027EDA24 */
static void __sinit_binary_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    binaryConstructRegister(&sStaticRegister);
}


/* 027ED8AC */
static void TParse_header_block__dt(JGadget::binary::TParse_header_block* object,u32 flags) {
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 027ED8C0 */
u32 JGadget::binary::TParse_header_block::parse_next(gptr<const void>* location,u32 flags) {
    if (location == NULL || (const void*)*location == NULL) return 0;
    gabi::Local<be<u32>> headerEnd;
    gabi::Local<be<u32>> blockEnd;
    u32 result = gabi::call_ptr<u32>(mVtable->mCheck,this,location,headerEnd.get(),flags) != 0;
    if (!(flags & 1) && !result) return result;
    while ((u32)*headerEnd != 0) {
        u32 parsed = gabi::call_ptr<u32>(mVtable->mBlock,this,location,blockEnd.get(),flags);
        result = parsed != 0 && result != 0;
        if (!(flags & 2) && !result) break;
        *headerEnd = (u32)*headerEnd - 1;
    }
    return result;
}
namespace JGadget { namespace binary {
/* 027ED9C0 */
const void* parseVariableUInt_16_32_following(const JGadget::binary::VariableUInt* buffer,be<u32>* first,be<u32>* second,be<u32>* bits) {
    gabi::Local<be<u32>> temporary;
    u32 value = buffer->mFirst;
    if (bits == NULL) bits = temporary.get();
    if (!(value & 0x8000)) {
        *bits = 16;
        *first = value;
        *second = buffer->mSecond;
        return (const u8*)buffer + 4;
    }
    *bits = 32;
    value = ((value & 0x7fff) << 16) | (u16)buffer->mSecond;
    *first = value;
    *second = buffer->mFollowing;
    return (const u8*)buffer + 8;
}

} }
using JGadget::binary::parseVariableUInt_16_32_following;

/* WWHD glue

 data sStaticBlock 104B4508
 data sStaticRegister 101F9820
 ext 028F026C void binaryConstructRegister(void*)
*/
