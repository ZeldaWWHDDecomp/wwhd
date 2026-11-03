/** JKRDecomp platform initialization; private decomp source. */
#include "bindings.h"
#include "JSystem/JKernel/JKRDecomp.h"
static JKRDecompStaticBlock sStaticBlock;
static u32 sStaticRegister;

/* 02842B8C */
static void __sinit_JKRDecomp_cpp() {
    sStaticBlock.mWords[2] = 0;
    sStaticBlock.mWords[0] = 0;
    sStaticBlock.mWords[3] = 0;
    sStaticBlock.mWords[1] = 0;
    JKRDecompConstructRegister(&sStaticRegister);
}


/* 028428FC */
s32 JKRDecomp::checkCompressed(const JKRCompressionHeader* buffer) {
    if (buffer->mMagic[0] == 'Y') {
        if (buffer->mMagic[1] != 'a' || buffer->mMagic[3] != '0') return 0;
        if (buffer->mMagic[2] == 'y') return 1;
        if (buffer->mMagic[2] == 'z') return 2;
    } else if (buffer->mMagic[0] == 'A' && buffer->mMagic[1] == 'S' && buffer->mMagic[2] == 'R') {
        // HD: ASR compression is recognized in addition to Yay0 and Yaz0.
        return 3;
    }
    return 0;
}
/* 02842974 */
static JKRDecompCommand* JKRDecompCommand__ct(JKRDecompCommand* command) {
    if (command == NULL) command = (JKRDecompCommand*)operator_new(sizeof(JKRDecompCommand));
    if (command != NULL) {
        JKRDecompInitMessageQueue(command->mMessageQueue, command->mMessageBuffer, 1);
        command->mSelf = command;
        command->mReplyQueue = NULL;
        command->mCallback = 0;
        command->mAramCommand = 0;
    }
    return command;
}


static JKRDecompQueue sDecompQueue;
/* 02842A44 */
void JKRDecompSendCommand(JKRDecompCommand* command) {
    if (JKRDecompSendMessage(&sDecompQueue,command,0) == 0)
        JKRDecompAssert("JKRDecomp.cpp",147,"Decomp MesgBuf FULL!");
}
/* 02842A94 */
JKRDecompCommand* JKRDecompOrderAsync(void* source,void* destination,u32 sourceLength,u32 destinationLength,void* callback) {
    JKRDecompCommand* command = JKRDecompPrepare(source,destination,sourceLength,destinationLength,callback);
    JKRDecompSendCommand(command);
    return command;
}
/* 02842AC8 */
u32 JKRDecompSync(JKRDecompCommand* command,s32 nonBlocking) {
    gabi::Local<JKRDecompMessage> message;
    if (nonBlocking == 0) {
        JKRDecompReceiveMessage(command->mMessageQueue,message.get(),1);
        return 1;
    }
    return JKRDecompReceiveMessage(command->mMessageQueue,message.get(),0) != 0;
}
/* 02842B24 */
void JKRDecompCommandDestroy(JKRDecompCommand* command,u32 flags) {
    if (command != NULL && (flags & 1)) operator_delete(command);
}
/* 02842B38 */
static u32 JKRDecompOrderSync(void* source,void* destination,u32 sourceLength,u32 destinationLength) {
    JKRDecompCommand* command = JKRDecompOrderAsync(source,destination,sourceLength,destinationLength,NULL);
    u32 result = JKRDecompSync(command,0);
    JKRDecompCommandDestroy(command,3);
    return result;
}

/* WWHD glue
 data sDecompQueue 101FAF60
 ext 028429D8 JKRDecompCommand* JKRDecompPrepare(void*,void*,u32,u32,void*)
 ext 0284B918 u32 JKRDecompSendMessage(void*,void*,u32)
 ext 0284B958 u32 JKRDecompReceiveMessage(void*,void*,u32)
 ext 0273AA24 void JKRDecompAssert(const char*,s32,const char*)
 str 10173AB0 "JKRDecomp.cpp"
 str 10173AC0 "Decomp MesgBuf FULL!"
 ext C0009D58 void JKRDecompInitMessageQueue(void*, void*, s32)
 data sStaticBlock 104B64D4
 data sStaticRegister 101FAF54
 ext 028F026C void JKRDecompConstructRegister(void*)
*/
