/** JSUMemoryStream platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JSupport/JSUMemoryStream.h"
static JSUMemoryStreamStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02842F64 */
static void __sinit_JSUMemoryStream_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JSUMemoryStreamConstructRegister(&sStaticRegister);
}


/* 02842E54 */
void JSUMemoryInputStream::setBuffer(const void* buffer,s32 length) {
    mBuffer = (const u8*)buffer;
    mLength = length;
    mPosition = 0;
}
/* 02842E68 */
u32 JSUMemoryInputStream::readData(void* data,s32 length) {
    s32 position = mPosition;
    if (position + length > (s32)mLength) length = (s32)mLength - position;
    if (length > 0) {
        const u8* source = (const u8*)mBuffer + position;
        JSUMemoryStreamCopy(data, source, length);
        mPosition = (s32)mPosition + length;
    }
    return length;
}
/* 02842EE0 */
s32 JSUMemoryInputStream::seekPos(s32 position,u32 origin) {
    s32 oldPosition = mPosition;
    switch (origin) {
    case 0: mPosition = position; break;
    case 1: mPosition = oldPosition + position; break;
    case 2: mPosition = (s32)mLength - position; break;
    }
    if ((s32)mPosition < 0) mPosition = 0;
    if ((s32)mPosition > (s32)mLength) mPosition = mLength;
    return (s32)mPosition - oldPosition;
}

/* WWHD glue
 ext C000A848 void* JSUMemoryStreamCopy(void*,const void*,u32)
 data sStaticBlock 104B6504
 data sStaticRegister 101FAFB4
 ext 028F026C void JSUMemoryStreamConstructRegister(void*)
*/
