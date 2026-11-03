#pragma once
#include "wwhd.h"
struct JSUMemoryStreamStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JSUMemoryStreamStaticBlock,16);
void JSUMemoryStreamConstructRegister(void*);


#include "JSystem/JSupport/JSUInputStream.h"
class JSUMemoryInputStream : public JSURandomInputStream {
public:
    void setBuffer(const void*,s32);
    u32 readData(void*,s32);
    s32 seekPos(s32,u32);
    gptr<const u8> mBuffer;
    be<s32> mLength;
    be<s32> mPosition;
};
WWHD_OFFSET(JSUMemoryInputStream,mBuffer,8);
WWHD_OFFSET(JSUMemoryInputStream,mLength,0xc);
WWHD_OFFSET(JSUMemoryInputStream,mPosition,0x10);
WWHD_SIZE(JSUMemoryInputStream,0x14);
void* JSUMemoryStreamCopy(void*,const void*,u32);
