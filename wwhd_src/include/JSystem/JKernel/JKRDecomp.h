#pragma once
#include "wwhd.h"
struct JKRDecompStaticBlock { be<u32> mWords[4]; };
WWHD_SIZE(JKRDecompStaticBlock,16);
void JKRDecompConstructRegister(void*);


struct JKRCompressionHeader { be<u8> mMagic[4]; };
WWHD_SIZE(JKRCompressionHeader,4);
class JKRDecomp { public: static s32 checkCompressed(const JKRCompressionHeader*); };
struct JKRDecompCommand {
    u8 mParameters[0x14];
    be<u32> mCallback;
    gptr<JKRDecompCommand> mSelf;
    gptr<void> mReplyQueue;
    be<u32> mAramCommand;
    u8 mQueuePadding[4];
    u8 mMessageQueue[0x3c];
    be<u32> mMessageBuffer[4];
};
WWHD_OFFSET(JKRDecompCommand,mCallback,0x14);
WWHD_OFFSET(JKRDecompCommand,mSelf,0x18);
WWHD_OFFSET(JKRDecompCommand,mReplyQueue,0x1c);
WWHD_OFFSET(JKRDecompCommand,mAramCommand,0x20);
WWHD_OFFSET(JKRDecompCommand,mMessageQueue,0x28);
WWHD_OFFSET(JKRDecompCommand,mMessageBuffer,0x64);
WWHD_SIZE(JKRDecompCommand,0x74);
void JKRDecompInitMessageQueue(void*, void*, s32);



struct JKRDecompQueue { u8 mStorage[0x3c]; };
struct JKRDecompMessage { be<u32> mValue; };
JKRDecompCommand* JKRDecompPrepare(void*,void*,u32,u32,void*);
u32 JKRDecompSendMessage(void*,void*,u32);
u32 JKRDecompReceiveMessage(void*,void*,u32);
void JKRDecompAssert(const char*,s32,const char*);
void JKRDecompSendCommand(JKRDecompCommand*);
JKRDecompCommand* JKRDecompOrderAsync(void*,void*,u32,u32,void*);
u32 JKRDecompSync(JKRDecompCommand*,s32);
void JKRDecompCommandDestroy(JKRDecompCommand*,u32);
WWHD_SIZE(JKRDecompQueue,0x3c);
WWHD_SIZE(JKRDecompMessage,4);
