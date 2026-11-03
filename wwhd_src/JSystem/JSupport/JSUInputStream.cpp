/** JSU input-stream state handling; private decomp source. */
#include "bindings.h"
#include "JSystem/JSupport/JSUInputStream.h"
static JSUStreamStaticBlock sStreamStaticBlock;
static u32 sStreamStaticRegister;
static JSUStreamVtable sInputStreamVtable;

/* 02842BDC */
static void JSUIosBase__dt(JSUInputStream* object, u32 flags) {
    if (object != NULL && (flags & 1)) operator_delete(object);
}
/* 02842BF0 */
static void JSUIosBase_reserved() {}
/* 02842BF4 */
static void JSUInputStream__dt(JSUInputStream* object, u32 flags) {
    if (object == NULL) return;
    object->mVtable = &sInputStreamVtable;
    if (flags & 1) operator_delete(object);
}
/* 02842C14 */
s32 JSUInputStream::read(void* buffer, s32 numBytes) {
    s32 bytesRead = gabi::call_ptr<s32>(mVtable->mReadData, this, buffer, numBytes);
    if (bytesRead != numBytes) mState = (u8)mState | 1;
    return bytesRead;
}
/* 02842C6C */
s32 JSURandomInputStream::peek(void* buffer, s32 numBytes) {
    s32 oldPos = gabi::call_ptr<s32>(mVtable->mPosition, this);
    s32 bytesRead = read(buffer,numBytes);
    if (bytesRead != 0) gabi::call_ptr<s32>(mVtable->mSeek, this, oldPos, 0);
    return bytesRead;
}
/* 02842D00 */
s32 JSURandomInputStream::seek(s32 position, s32 origin) {
    s32 result = gabi::call_ptr<s32>(mVtable->mSeek, this, position, origin);
    mState = (u8)mState & ~1;
    return result;
}
/* 02842D44 */
s32 JSURandomInputStream::align(s32 alignment) {
    u32 current = gabi::call_ptr<u32>(mVtable->mPosition, this);
    u32 position = (current + (u32)alignment - 1) & ~((u32)alignment - 1);
    s32 offset = position - current;
    if (offset != 0) {
        s32 result = gabi::call_ptr<s32>(mVtable->mSeek, this, position, 0);
        if (result != offset) mState = (u8)mState | 1;
    }
    return offset;
}
/* 02842DD0 */
s32 JSURandomInputStream::skip(s32 count) {
    s32 result = gabi::call_ptr<s32>(mVtable->mSeek, this, count, 1);
    if (result != count) mState = (u8)mState | 1;
    return result;
}
/* 02842E2C */
static void __sinit_JSUInputStream_cpp() {
    sStreamStaticBlock.mWords[2] = 0;
    sStreamStaticBlock.mWords[0] = 0;
    sStreamStaticBlock.mWords[3] = 0;
    sStreamStaticBlock.mWords[1] = 0;
    JSUStreamConstructRegister(&sStreamStaticRegister);
}
/* WWHD glue
 data sStreamStaticBlock 104B64F4
 data sStreamStaticRegister 101FAFA8
 data sInputStreamVtable 10173B1C
 ext 028F026C void JSUStreamConstructRegister(void*)
*/
