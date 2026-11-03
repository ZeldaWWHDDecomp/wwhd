#pragma once
#include "JSystem/J2DGraph/J2DGrafContext.h"
struct J2DPrint {
    static be<u8>* setBuffer(u32);
};
struct J2DPrintBuffer { gptr<be<u8>> mText; be<u32> mSize; be<u8> mHeapFlag; u8 padding[3]; };
struct J2DPrintStaticState { be<u32> words[4]; };
WWHD_OFFSET(J2DPrintBuffer,mSize,4); WWHD_OFFSET(J2DPrintBuffer,mHeapFlag,8);
WWHD_SIZE(J2DPrintBuffer,12); WWHD_SIZE(J2DPrintStaticState,16);
void* printBufferAllocate(u32,void*,s32);
void printBufferDelete(void*);
void* printBufferSystemHeap();
void printBufferAssert(const char*,u32,const char*);
void printBufferRegisterStatic(void*);

void* printColorAllocate(u32);
