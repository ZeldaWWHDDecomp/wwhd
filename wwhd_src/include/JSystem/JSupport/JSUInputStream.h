/** Stream state precedes the GHS vtable in WWHD. */
#pragma once
#include "wwhd.h"
struct JSUStreamVtable {
    be<u32> mType[2];
    be<u32> mDestructor[2];
    be<u32> mAvailable[2];
    be<u32> mSkip[2];
    be<u32> mReadDataThis;
    be<u32> mReadData;
    be<u32> mLength[2];
    be<u32> mPositionThis;
    be<u32> mPosition;
    be<u32> mSeekThis;
    be<u32> mSeek;
};
class JSUInputStream {
public:
    s32 read(void*, s32);
    be<u8> mState;
    u8 mPadding[3];
    gptr<JSUStreamVtable> mVtable;
};
class JSURandomInputStream : public JSUInputStream {
public:
    s32 peek(void*,s32);
    s32 seek(s32,s32);
    s32 align(s32);
    s32 skip(s32);
};
struct JSUStreamStaticBlock { be<u32> mWords[4]; };
void JSUStreamConstructRegister(void*);
WWHD_OFFSET(JSUInputStream,mState,0);
WWHD_OFFSET(JSUInputStream,mVtable,4);
WWHD_SIZE(JSUInputStream,8);
WWHD_SIZE(JSURandomInputStream,8);
WWHD_SIZE(JSUStreamVtable,0x40);
WWHD_SIZE(JSUStreamStaticBlock,16);
